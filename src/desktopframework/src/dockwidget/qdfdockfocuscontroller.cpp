#include "private/qdfdockfocuscontroller_p.h"
#include "qdfdockwidgethelper.h"
#include <QAbstractButton>
#include <QApplication>
#include <QPointer>
#include <QWindow>
#include <algorithm>
#include <dockwidget/qdfdockareatitlebar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockfocuscontroller.h>
#include <dockwidget/qdfdockwidget.h>
#include <dockwidget/qdfdockwidgettab.h>
#include <dockwidget/qdffloatingdockcontainer.h>
#include <iostream>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    #include "linux/qdffloatingwidgettitlebar.h"
#endif

QDF_USE_NAMESPACE

static const char *const FocusedDockWidgetProperty = "FocusedDockWidget";

static void updateDockWidgetFocusStyle(QdfDockWidget *dockWidget, bool focused)
{
    dockWidget->setProperty("focused", focused);
    dockWidget->tabWidget()->setProperty("focused", focused);
    dockWidget->tabWidget()->updateStyle();
    internal::repolishStyle(dockWidget);
}

static void updateDockAreaFocusStyle(QdfDockAreaWidget *dockArea, bool focused)
{
    dockArea->setProperty("focused", focused);
    internal::repolishStyle(dockArea);
    internal::repolishStyle(dockArea->titleBar());
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
static void updateFloatingWidgetFocusStyle(QdfFloatingDockContainer *floatingWidget, bool focused)
{
    if (floatingWidget->hasNativeTitleBar())
    {
        return;
    }
    auto titleBar = qobject_cast<QdfFloatingWidgetTitleBar *>(floatingWidget->titleBarWidget());
    if (!titleBar)
    {
        return;
    }
    titleBar->setProperty("focused", focused);
    titleBar->updateStyle();
}
#endif

void QdfDockFocusControllerPrivate::updateDockWidgetFocus(QdfDockWidget *dockWidget)
{
    QDF_Q(QdfDockFocusController)
    if (!dockWidget->features().testFlag(QdfDockWidget::DockWidgetFocusable))
    {
        return;
    }

    QWindow *window = nullptr;
    auto dockContainer = dockWidget->dockContainer();
    if (dockContainer)
    {
        window = dockContainer->window()->windowHandle();
    }

    if (window)
    {
        window->setProperty(FocusedDockWidgetProperty, QVariant::fromValue(QPointer<QdfDockWidget>(dockWidget)));
    }
    QdfDockAreaWidget *newFocusedDockArea = nullptr;
    if (focusedDockWidget)
    {
        updateDockWidgetFocusStyle(focusedDockWidget, false);
    }

    QdfDockWidget *old = focusedDockWidget;
    focusedDockWidget = dockWidget;
    updateDockWidgetFocusStyle(focusedDockWidget, true);
    newFocusedDockArea = focusedDockWidget->dockAreaWidget();
    if (newFocusedDockArea && (focusedArea != newFocusedDockArea))
    {
        if (focusedArea)
        {
            QObject::disconnect(focusedArea, SIGNAL(viewToggled(bool)), q, SLOT(onFocusedDockAreaViewToggled(bool)));
            updateDockAreaFocusStyle(focusedArea, false);
        }

        focusedArea = newFocusedDockArea;
        updateDockAreaFocusStyle(focusedArea, true);
        QObject::connect(focusedArea, SIGNAL(viewToggled(bool)), q, SLOT(onFocusedDockAreaViewToggled(bool)));
    }

    QdfFloatingDockContainer *newFloatingWidget = nullptr;
    dockContainer = focusedDockWidget->dockContainer();
    if (dockContainer)
    {
        newFloatingWidget = dockContainer->floatingWidget();
    }

    if (newFloatingWidget)
    {
        newFloatingWidget->setProperty(FocusedDockWidgetProperty, QVariant::fromValue(QPointer<QdfDockWidget>(dockWidget)));
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (floatingWidget != newFloatingWidget)
    {
        if (floatingWidget)
        {
            updateFloatingWidgetFocusStyle(floatingWidget, false);
        }
        floatingWidget = newFloatingWidget;

        if (floatingWidget)
        {
            updateFloatingWidgetFocusStyle(floatingWidget, true);
        }
    }
#endif

    if (old == dockWidget && !forceFocusChangedSignal)
    {
        return;
    }

    forceFocusChangedSignal = false;
    if (dockWidget->isVisible())
    {
        Q_EMIT dockManager->focusedDockWidgetChanged(old, dockWidget);
    }
    else
    {
        oldFocusedDockWidget = old;
        QObject::connect(dockWidget, SIGNAL(visibilityChanged(bool)), q, SLOT(onDockWidgetVisibilityChanged(bool)));
    }
}

void QdfDockFocusController::onDockWidgetVisibilityChanged(bool visible)
{
    QDF_D(QdfDockFocusController)
    auto obj = sender();
    auto dockWidget = qobject_cast<QdfDockWidget *>(obj);
    disconnect(obj, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisibilityChanged(bool)));
    if (dockWidget && visible)
    {
        Q_EMIT d->dockManager->focusedDockWidgetChanged(d->oldFocusedDockWidget, dockWidget);
    }
}

QdfDockFocusController::QdfDockFocusController(QdfDockManager *dockManager)
    : QObject(dockManager)
{
    QDF_INIT_PRIVATE(QdfDockFocusController)
    QDF_D(QdfDockFocusController)
    d->dockManager = dockManager;
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(onApplicationFocusChanged(QWidget *, QWidget *)));
    connect(QApplication::instance(), SIGNAL(focusWindowChanged(QWindow *)),
            this, SLOT(onFocusWindowChanged(QWindow *)));
    connect(d->dockManager, SIGNAL(stateRestored()), SLOT(onStateRestored()));
}

QdfDockFocusController::~QdfDockFocusController()
{
    QDF_FINI_PRIVATE()
}

void QdfDockFocusController::onFocusWindowChanged(QWindow *focusWindow)
{
    QDF_D(QdfDockFocusController)
    if (!focusWindow)
    {
        return;
    }

    auto vDockWidget = focusWindow->property(FocusedDockWidgetProperty);
    if (!vDockWidget.isValid())
    {
        return;
    }

    auto DockWidget = vDockWidget.value<QPointer<QdfDockWidget>>();
    if (!DockWidget)
    {
        return;
    }

    d->updateDockWidgetFocus(DockWidget);
}

void QdfDockFocusController::onApplicationFocusChanged(QWidget *focusedOld, QWidget *focusedNow)
{
    QDF_D(QdfDockFocusController)
    Q_UNUSED(focusedOld);

    if (d->dockManager->isRestoringState())
    {
        return;
    }

    if (!focusedNow)
    {
        return;
    }

    QdfDockWidget *DockWidget = qobject_cast<QdfDockWidget *>(focusedNow);
    if (!DockWidget)
    {
        DockWidget = internal::findParent<QdfDockWidget *>(focusedNow);
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (!DockWidget)
    {
        return;
    }
#else
    if (!DockWidget || DockWidget->tabWidget()->isHidden())
    {
        return;
    }
#endif

    d->updateDockWidgetFocus(DockWidget);
}


void QdfDockFocusController::setDockWidgetTabFocused(QdfDockWidgetTab *tab)
{
    QDF_D(QdfDockFocusController)
    auto dockWidget = tab->dockWidget();
    if (dockWidget)
    {
        d->updateDockWidgetFocus(dockWidget);
    }
}


void QdfDockFocusController::clearDockWidgetFocus(QdfDockWidget *dockWidget)
{
    dockWidget->clearFocus();
    updateDockWidgetFocusStyle(dockWidget, false);
}


void QdfDockFocusController::setDockWidgetFocused(QdfDockWidget *focusedNow)
{
    QDF_D(QdfDockFocusController)
    d->updateDockWidgetFocus(focusedNow);
}


void QdfDockFocusController::onFocusedDockAreaViewToggled(bool open)
{
    QDF_D(QdfDockFocusController)
    if (d->dockManager->isRestoringState())
    {
        return;
    }

    QdfDockAreaWidget *dockArea = qobject_cast<QdfDockAreaWidget *>(sender());
    if (!dockArea || open)
    {
        return;
    }
    auto container = dockArea->dockContainer();
    auto openedDockAreas = container->openedDockAreas();
    if (openedDockAreas.isEmpty())
    {
        return;
    }

    d->updateDockWidgetFocus(openedDockAreas[0]->currentDockWidget());
}


void QdfDockFocusController::notifyWidgetOrAreaRelocation(QWidget *droppedWidget)
{
    QDF_D(QdfDockFocusController)
    if (d->dockManager->isRestoringState())
    {
        return;
    }

    QdfDockWidget *dockWidget = qobject_cast<QdfDockWidget *>(droppedWidget);
    if (!dockWidget)
    {
        QdfDockAreaWidget *dockArea = qobject_cast<QdfDockAreaWidget *>(droppedWidget);
        if (dockArea)
        {
            dockWidget = dockArea->currentDockWidget();
        }
    }

    if (!dockWidget)
    {
        return;
    }

    d->forceFocusChangedSignal = true;
    QdfDockManager::setWidgetFocus(dockWidget);
}

void QdfDockFocusController::notifyFloatingWidgetDrop(QdfFloatingDockContainer *floatingWidget)
{
    QDF_D(QdfDockFocusController)
    if (!floatingWidget || d->dockManager->isRestoringState())
    {
        return;
    }

    auto vDockWidget = floatingWidget->property(FocusedDockWidgetProperty);
    if (!vDockWidget.isValid())
    {
        return;
    }

    auto DockWidget = vDockWidget.value<QPointer<QdfDockWidget>>();
    if (DockWidget)
    {
        DockWidget->dockAreaWidget()->setCurrentDockWidget(DockWidget);
        QdfDockManager::setWidgetFocus(DockWidget);
    }
}

void QdfDockFocusController::onStateRestored()
{
    QDF_D(QdfDockFocusController)
    if (d->focusedDockWidget)
    {
        updateDockWidgetFocusStyle(d->focusedDockWidget, false);
    }
}

QdfDockWidget *QdfDockFocusController::focusedDockWidget() const
{
    QDF_D(const QdfDockFocusController)
    return d->focusedDockWidget.data();
}