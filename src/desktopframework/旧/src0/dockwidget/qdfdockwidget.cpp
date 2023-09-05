#include "private/qdfdockwidget_p.h"
#include "qdfdockcomponentsfactory.h"
#include "qdfdockwidgethelper.h"
#include <QAction>
#include <QBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QGuiApplication>
#include <QPointer>
#include <QScreen>
#include <QScrollArea>
#include <QSplitter>
#include <QStack>
#include <QTextStream>
#include <QToolBar>
#include <QWindow>
#include <QXmlStreamWriter>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockwidget.h>
#include <dockwidget/qdfdockwidgettab.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

QdfDockWidgetPrivate::QdfDockWidgetPrivate()
{
}

void QdfDockWidgetPrivate::showDockWidget()
{
    QDF_Q(QdfDockWidget)
    if (!widget)
    {
        if (!createWidgetFromFactory())
        {
            Q_ASSERT(!features.testFlag(QdfDockWidget::DeleteContentOnClose) && "DeleteContentOnClose flag was set, but the widget "
                                                                                "factory is missing or it doesn't return a valid QWidget.");
            return;
        }
    }

    if (!dockArea)
    {
        QdfFloatingDockContainer *floatingWidget = new QdfFloatingDockContainer(q);
        floatingWidget->resize(widget ? widget->sizeHint() : q->sizeHint());
        tabWidget->show();
        floatingWidget->show();
    }
    else
    {
        dockArea->setCurrentDockWidget(q);
        dockArea->toggleView(true);
        tabWidget->show();
        QSplitter *splitter = internal::findParent<QSplitter *>(dockArea);
        while (splitter && !splitter->isVisible() && !dockArea->autoHide())
        {
            splitter->show();
            splitter = internal::findParent<QSplitter *>(splitter);
        }

        QdfDockContainerWidget *container = dockArea->dockContainer();
        if (container->isFloating())
        {
            QdfFloatingDockContainer *floatingWidget = internal::findParent<
                    QdfFloatingDockContainer *>(container);
            floatingWidget->show();
        }

        if (container->openedDockWidgets().count() == 0 && dockArea->autoHide() && !dockManager->isRestoringState())
        {
            dockArea->autoHideDockContainer()->moveContentsToParent();
        }
    }
}

void QdfDockWidgetPrivate::hideDockWidget()
{
    tabWidget->hide();
    updateParentDockArea();

    closeAutoHideDockWidgetsIfNeeded();

    if (features.testFlag(QdfDockWidget::DeleteContentOnClose))
    {
        widget->deleteLater();
        widget = nullptr;
    }
}

void QdfDockWidgetPrivate::updateParentDockArea()
{
    QDF_Q(QdfDockWidget)
    if (!dockArea)
    {
        return;
    }

    if (dockArea->currentDockWidget() != q)
    {
        return;
    }

    auto NextDockWidget = dockArea->nextOpenDockWidget(q);
    if (NextDockWidget)
    {
        dockArea->setCurrentDockWidget(NextDockWidget);
    }
    else
    {
        dockArea->hideAreaWithNoVisibleContent();
    }
}

void QdfDockWidgetPrivate::closeAutoHideDockWidgetsIfNeeded()
{
    QDF_Q(QdfDockWidget)
    auto DockContainer = q->dockContainer();
    if (!DockContainer)
    {
        return;
    }

    if (q->dockManager()->isRestoringState())
    {
        return;
    }

    if (!DockContainer->openedDockWidgets().isEmpty())
    {
        return;
    }

    for (auto autoHideWidget: DockContainer->autoHideWidgets())
    {
        auto DockWidget = autoHideWidget->dockWidget();
        if (DockWidget == q)
        {
            continue;
        }

        DockWidget->toggleView(false);
    }
}

void QdfDockWidgetPrivate::setupToolBar()
{
    QDF_Q(QdfDockWidget)
    toolBar = new QToolBar(q);
    toolBar->setObjectName("dockWidgetToolBar");
    layout->insertWidget(0, toolBar);
    toolBar->setIconSize(QSize(16, 16));
    toolBar->toggleViewAction()->setEnabled(false);
    toolBar->toggleViewAction()->setVisible(false);
    q->connect(q, SIGNAL(topLevelChanged(bool)), SLOT(setToolbarFloatingStyle(bool)));
}

void QdfDockWidgetPrivate::setupScrollArea()
{
    QDF_Q(QdfDockWidget)
    scrollArea = new QScrollArea(q);
    scrollArea->setObjectName("dockWidgetScrollArea");
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);
}

bool QdfDockWidgetPrivate::createWidgetFromFactory()
{
    QDF_Q(QdfDockWidget)
    if (!features.testFlag(QdfDockWidget::DeleteContentOnClose))
    {
        return false;
    }

    if (!factory)
    {
        return false;
    }

    QWidget *w = factory->createWidget(q);
    if (!w)
    {
        return false;
    }

    q->setWidget(w, factory->insertMode);
    return true;
}

QdfDockWidget::QdfDockWidget(const QString &title, QWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockWidget)
    QDF_D(QdfDockWidget)
    d->layout = new QBoxLayout(QBoxLayout::TopToBottom);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    setLayout(d->layout);
    setWindowTitle(title);
    setObjectName(title);

    d->tabWidget = QdfDockComponentsFactory::instance()->createDockWidgetTab(this);

    d->toggleViewAction = new QAction(title, this);
    d->toggleViewAction->setCheckable(true);
    connect(d->toggleViewAction, SIGNAL(triggered(bool)), this,
            SLOT(toggleView(bool)));
    setToolbarFloatingStyle(false);

    if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
    {
        setFocusPolicy(Qt::ClickFocus);
    }
}

QdfDockWidget::~QdfDockWidget()
{
    QDF_FINI_PRIVATE()
}

void QdfDockWidget::setToggleViewActionChecked(bool checked)
{
    QDF_D(QdfDockWidget)
    QAction *action = d->toggleViewAction;
    action->blockSignals(true);
    action->setChecked(checked);
    action->blockSignals(false);
}

void QdfDockWidget::setWidget(QWidget *widget, InsertMode InsertMode)
{
    QDF_D(QdfDockWidget)
    if (d->widget)
    {
        takeWidget();
    }

    auto ScrollAreaWidget = qobject_cast<QAbstractScrollArea *>(widget);
    if (ScrollAreaWidget || ForceNoScrollArea == InsertMode)
    {
        d->layout->addWidget(widget);
        if (ScrollAreaWidget && ScrollAreaWidget->viewport())
        {
            ScrollAreaWidget->viewport()->setProperty("dockWidgetContent", true);
        }
    }
    else
    {
        d->setupScrollArea();
        d->scrollArea->setWidget(widget);
    }

    d->widget = widget;
    d->widget->setProperty("dockWidgetContent", true);
}

void QdfDockWidget::setWidgetFactory(FactoryFunc createWidget, InsertMode insertMode)
{
    QDF_D(QdfDockWidget)
    if (d->factory)
    {
        delete d->factory;
    }

    d->factory = new QdfDockWidgetPrivate::WidgetFactory{createWidget, insertMode};
}

QWidget *QdfDockWidget::takeWidget()
{
    QDF_D(QdfDockWidget)
    QWidget *w = nullptr;
    if (d->scrollArea)
    {
        d->layout->removeWidget(d->scrollArea);
        w = d->scrollArea->takeWidget();
        delete d->scrollArea;
        d->scrollArea = nullptr;
        d->widget = nullptr;
    }
    else if (d->widget)
    {
        d->layout->removeWidget(d->widget);
        w = d->widget;
        d->widget = nullptr;
    }

    if (w)
    {
        w->setParent(nullptr);
    }
    return w;
}

QWidget *QdfDockWidget::widget() const
{
    QDF_D(const QdfDockWidget)
    return d->widget;
}

QdfDockWidgetTab *QdfDockWidget::tabWidget() const
{
    QDF_D(const QdfDockWidget)
    return d->tabWidget;
}


void QdfDockWidget::setFeatures(DockWidgetFeatures features)
{
    QDF_D(QdfDockWidget)
    if (d->features == features)
    {
        return;
    }
    d->features = features;
    Q_EMIT featuresChanged(d->features);
    d->tabWidget->onDockWidgetFeaturesChanged();
    if (QdfDockAreaWidget *DockArea = dockAreaWidget())
    {
        DockArea->onDockWidgetFeaturesChanged();
    }
}

void QdfDockWidget::setFeature(DockWidgetFeature flag, bool on)
{
    auto Features = features();
    internal::setFlag(Features, flag, on);
    setFeatures(Features);
}

QdfDockWidget::DockWidgetFeatures QdfDockWidget::features() const
{
    QDF_D(const QdfDockWidget)
    return d->features;
}

QdfDockManager *QdfDockWidget::dockManager() const
{
    QDF_D(const QdfDockWidget)
    return d->dockManager;
}

void QdfDockWidget::setDockManager(QdfDockManager *dockManager)
{
    QDF_D(QdfDockWidget)
    d->dockManager = dockManager;
}

QdfDockContainerWidget *QdfDockWidget::dockContainer() const
{
    QDF_D(const QdfDockWidget)
    if (d->dockArea)
    {
        return d->dockArea->dockContainer();
    }
    else
    {
        return 0;
    }
}

QdfFloatingDockContainer *QdfDockWidget::floatingDockContainer() const
{
    auto widget = dockContainer();
    return widget ? widget->floatingWidget() : nullptr;
}

QdfDockAreaWidget *QdfDockWidget::dockAreaWidget() const
{
    QDF_D(const QdfDockWidget)
    return d->dockArea;
}

QdfAutoHideTab *QdfDockWidget::sideTabWidget() const
{
    QDF_D(const QdfDockWidget)
    return d->sideTabWidget;
}

void QdfDockWidget::setSideTabWidget(QdfAutoHideTab *sideTab)
{
    QDF_D(QdfDockWidget)
    d->sideTabWidget = sideTab;
}

bool QdfDockWidget::autoHide() const
{
    QDF_D(const QdfDockWidget)
    return !d->sideTabWidget.isNull();
}
QdfAutoHideDockContainer *QdfDockWidget::autoHideDockContainer() const
{
    QDF_D(const QdfDockWidget)
    if (!d->dockArea)
    {
        return nullptr;
    }

    return d->dockArea->autoHideDockContainer();
}

bool QdfDockWidget::isFloating() const
{
    if (!isInFloatingContainer())
    {
        return false;
    }

    return dockContainer()->topLevelDockWidget() == this;
}

bool QdfDockWidget::isInFloatingContainer() const
{
    auto Container = dockContainer();
    if (!Container)
    {
        return false;
    }

    if (!Container->isFloating())
    {
        return false;
    }

    return true;
}

bool QdfDockWidget::isClosed() const
{
    QDF_D(const QdfDockWidget)
    return d->closed;
}

QAction *QdfDockWidget::toggleViewAction() const
{
    QDF_D(const QdfDockWidget)
    return d->toggleViewAction;
}

void QdfDockWidget::setToggleViewActionMode(ToggleViewActionMode mode)
{
    QDF_D(QdfDockWidget)
    if (ActionModeToggle == mode)
    {
        d->toggleViewAction->setCheckable(true);
        d->toggleViewAction->setIcon(QIcon());
    }
    else
    {
        d->toggleViewAction->setCheckable(false);
        d->toggleViewAction->setIcon(d->tabWidget->icon());
    }
}

void QdfDockWidget::setMinimumSizeHintMode(MinimumSizeHintMode mode)
{
    QDF_D(QdfDockWidget)
    d->minimumSizeHintMode = mode;
}

QdfDockWidget::MinimumSizeHintMode QdfDockWidget::minimumSizeHintMode() const
{
    QDF_D(const QdfDockWidget)
    return d->minimumSizeHintMode;
}

bool QdfDockWidget::isCentralWidget() const
{
    return dockManager()->centralWidget() == this;
}

void QdfDockWidget::toggleView(bool open)
{
    QDF_D(QdfDockWidget)
    QAction *action = qobject_cast<QAction *>(sender());
    if (action == d->toggleViewAction && !d->toggleViewAction->isCheckable())
    {
        open = true;
    }

    auto containerWidget = autoHideDockContainer();
    if (d->closed != !open)
    {
        toggleViewInternal(open);
    }
    else if (open && d->dockArea && !containerWidget)
    {
        raise();
    }

    if (open && containerWidget)
    {
        containerWidget->collapseView(false);
    }
}

void QdfDockWidget::toggleViewInternal(bool open)
{
    QDF_D(QdfDockWidget)
    QdfDockContainerWidget *DockContainer = dockContainer();
    QdfDockWidget *topLevelDockWidgetBefore = DockContainer
                                                      ? DockContainer->topLevelDockWidget()
                                                      : nullptr;

    d->closed = !open;

    if (open)
    {
        d->showDockWidget();
    }
    else
    {
        d->hideDockWidget();
    }

    d->toggleViewAction->blockSignals(true);
    d->toggleViewAction->setChecked(open);
    d->toggleViewAction->blockSignals(false);
    if (d->dockArea)
    {
        d->dockArea->toggleDockWidgetView(this, open);
    }

    if (d->dockArea->autoHide())
    {
        d->dockArea->autoHideDockContainer()->toggleView(open);
    }

    if (open && topLevelDockWidgetBefore)
    {
        QdfDockWidget::emitTopLevelEventForWidget(topLevelDockWidgetBefore, false);
    }

    DockContainer = dockContainer();
    QdfDockWidget *TopLevelDockWidgetAfter = DockContainer
                                                     ? DockContainer->topLevelDockWidget()
                                                     : nullptr;
    QdfDockWidget::emitTopLevelEventForWidget(TopLevelDockWidgetAfter, true);
    QdfFloatingDockContainer *FloatingContainer = DockContainer
                                                          ? DockContainer->floatingWidget()
                                                          : nullptr;
    if (FloatingContainer)
    {
        FloatingContainer->updateWindowTitle();
    }

    if (!open)
    {
        Q_EMIT closed();
    }
    Q_EMIT viewToggled(open);
}

void QdfDockWidget::setDockArea(QdfDockAreaWidget *dockArea)
{
    QDF_D(QdfDockWidget)
    d->dockArea = dockArea;
    d->toggleViewAction->setChecked(dockArea != nullptr && !this->isClosed());
    setParent(dockArea);
}

void QdfDockWidget::flagAsUnassigned()
{
    QDF_D(QdfDockWidget)
    d->closed = true;
    setParent(d->dockManager);
    setVisible(false);
    setDockArea(nullptr);
    tabWidget()->setParent(this);
}

bool QdfDockWidget::event(QEvent *event)
{
    QDF_D(QdfDockWidget)
    switch (event->type())
    {
        case QEvent::Hide:
            Q_EMIT visibilityChanged(false);
            break;

        case QEvent::Show:
            Q_EMIT visibilityChanged(geometry().right() >= 0 && geometry().bottom() >= 0);
            break;

        case QEvent::WindowTitleChange:
            {
                const auto title = windowTitle();
                if (d->tabWidget)
                {
                    d->tabWidget->setText(title);
                }
                if (d->sideTabWidget)
                {
                    d->sideTabWidget->setText(title);
                }
                if (d->toggleViewAction)
                {
                    d->toggleViewAction->setText(title);
                }
                if (d->dockArea)
                {
                    d->dockArea->markTitleBarMenuOutdated();// update tabs menu
                }

                auto FloatingWidget = floatingDockContainer();
                if (FloatingWidget)
                {
                    FloatingWidget->updateWindowTitle();
                }
                Q_EMIT titleChanged(title);
            }
            break;

        default:
            break;
    }

    return QFrame::event(event);
}

#ifndef QT_NO_TOOLTIP

void QdfDockWidget::setTabToolTip(const QString &text)
{
    QDF_D(QdfDockWidget)
    if (d->tabWidget)
    {
        d->tabWidget->setToolTip(text);
    }
    if (d->toggleViewAction)
    {
        d->toggleViewAction->setToolTip(text);
    }
    if (d->dockArea)
    {
        d->dockArea->markTitleBarMenuOutdated();// update tabs menu
    }
}
#endif

void QdfDockWidget::setIcon(const QIcon &icon)
{
    QDF_D(QdfDockWidget)
    d->tabWidget->setIcon(icon);

    if (d->sideTabWidget)
    {
        d->sideTabWidget->setIcon(icon);
    }

    if (!d->toggleViewAction->isCheckable())
    {
        d->toggleViewAction->setIcon(icon);
    }
}

QIcon QdfDockWidget::icon() const
{
    QDF_D(const QdfDockWidget)
    return d->tabWidget->icon();
}

QToolBar *QdfDockWidget::toolBar() const
{
    QDF_D(const QdfDockWidget)
    return d->toolBar;
}

QToolBar *QdfDockWidget::createDefaultToolBar()
{
    QDF_D(QdfDockWidget)
    if (!d->toolBar)
    {
        d->setupToolBar();
    }

    return d->toolBar;
}

void QdfDockWidget::setToolBar(QToolBar *ToolBar)
{
    QDF_D(QdfDockWidget)
    if (d->toolBar)
    {
        delete d->toolBar;
    }

    d->toolBar = ToolBar;
    d->layout->insertWidget(0, d->toolBar);
    this->connect(this, SIGNAL(topLevelChanged(bool)), SLOT(setToolbarFloatingStyle(bool)));
    setToolbarFloatingStyle(isFloating());
}

void QdfDockWidget::setToolBarStyle(Qt::ToolButtonStyle style, DockWidgetState state)
{
    QDF_D(QdfDockWidget)
    if (StateFloating == state)
    {
        d->toolBarStyleFloating = style;
    }
    else
    {
        d->toolBarStyleDocked = style;
    }

    setToolbarFloatingStyle(isFloating());
}

Qt::ToolButtonStyle QdfDockWidget::toolBarStyle(DockWidgetState state) const
{
    QDF_D(const QdfDockWidget)
    if (StateFloating == state)
    {
        return d->toolBarStyleFloating;
    }
    else
    {
        return d->toolBarStyleDocked;
    }
}

void QdfDockWidget::setToolBarIconSize(const QSize &iconSize, DockWidgetState state)
{
    QDF_D(QdfDockWidget)
    if (StateFloating == state)
    {
        d->toolBarIconSizeFloating = iconSize;
    }
    else
    {
        d->toolBarIconSizeDocked = iconSize;
    }

    setToolbarFloatingStyle(isFloating());
}

QSize QdfDockWidget::toolBarIconSize(DockWidgetState state) const
{
    QDF_D(const QdfDockWidget)
    if (StateFloating == state)
    {
        return d->toolBarIconSizeFloating;
    }
    else
    {
        return d->toolBarIconSizeDocked;
    }
}

void QdfDockWidget::setToolbarFloatingStyle(bool floating)
{
    QDF_D(QdfDockWidget)
    if (!d->toolBar)
    {
        return;
    }

    auto iconSize = floating ? d->toolBarIconSizeFloating : d->toolBarIconSizeDocked;
    if (iconSize != d->toolBar->iconSize())
    {
        d->toolBar->setIconSize(iconSize);
    }

    auto ButtonStyle = floating ? d->toolBarStyleFloating : d->toolBarStyleDocked;
    if (ButtonStyle != d->toolBar->toolButtonStyle())
    {
        d->toolBar->setToolButtonStyle(ButtonStyle);
    }
}

void QdfDockWidget::emitTopLevelEventForWidget(QdfDockWidget *topLevelDockWidget, bool floating)
{
    if (topLevelDockWidget)
    {
        topLevelDockWidget->dockAreaWidget()->updateTitleBarVisibility();
        topLevelDockWidget->emitTopLevelChanged(floating);
    }
}

void QdfDockWidget::emitTopLevelChanged(bool floating)
{
    QDF_D(QdfDockWidget)
    if (floating != d->isFloatingTopLevel)
    {
        d->isFloatingTopLevel = floating;
        Q_EMIT topLevelChanged(d->isFloatingTopLevel);
    }
}

void QdfDockWidget::setClosedState(bool closed)
{
    QDF_D(QdfDockWidget)
    d->closed = closed;
}

QSize QdfDockWidget::minimumSizeHint() const
{
    QDF_D(const QdfDockWidget)
    if (!d->widget)
    {
        return QSize(60, 40);
    }

    switch (d->minimumSizeHintMode)
    {
        case MinimumSizeHintFromDockWidget:
            return QSize(60, 40);
        case MinimumSizeHintFromContent:
            return d->widget->minimumSizeHint();
        case MinimumSizeHintFromDockWidgetMinimumSize:
            return minimumSize();
        case MinimumSizeHintFromContentMinimumSize:
            return d->widget->minimumSize();
    }

    return d->widget->minimumSizeHint();
}

void QdfDockWidget::setFloating()
{
    QDF_D(QdfDockWidget)
    if (isClosed())
    {
        return;
    }
    d->tabWidget->detachDockWidget();
}

void QdfDockWidget::deleteDockWidget()
{
    QDF_D(QdfDockWidget)
    auto manager = dockManager();
    if (manager)
    {
        manager->removeDockWidget(this);
    }
    deleteLater();
    d->closed = true;
}

void QdfDockWidget::closeDockWidget()
{
    closeDockWidgetInternal(true);
}

bool QdfDockWidget::closeDockWidgetInternal(bool forceClose)
{
    QDF_D(QdfDockWidget)
    if (!forceClose)
    {
        Q_EMIT closeRequested();
    }

    if (!forceClose && features().testFlag(QdfDockWidget::CustomCloseHandling))
    {
        return false;
    }

    if (features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose))
    {
        if (isFloating())
        {
            QdfFloatingDockContainer *FloatingWidget = internal::findParent<
                    QdfFloatingDockContainer *>(this);
            if (FloatingWidget->dockWidgets().count() == 1)
            {
                FloatingWidget->deleteLater();
            }
            else
            {
                FloatingWidget->hide();
            }
        }
        if (d->dockArea && d->dockArea->autoHide())
        {
            d->dockArea->autoHideDockContainer()->cleanupAndDelete();
        }
        deleteDockWidget();
        Q_EMIT closed();
    }
    else
    {
        toggleView(false);
    }

    return true;
}

void QdfDockWidget::setTitleBarActions(QList<QAction *> actions)
{
    QDF_D(QdfDockWidget)
    d->titleBarActions = actions;
}

QList<QAction *> QdfDockWidget::titleBarActions() const
{
    QDF_D(const QdfDockWidget)
    return d->titleBarActions;
}

void QdfDockWidget::showFullScreen()
{
    if (isFloating())
    {
        dockContainer()->floatingWidget()->showFullScreen();
    }
    else
    {
        QFrame::showFullScreen();
    }
}

void QdfDockWidget::showNormal()
{
    if (isFloating())
    {
        dockContainer()->floatingWidget()->showNormal();
    }
    else
    {
        QFrame::showNormal();
    }
}

bool QdfDockWidget::isFullScreen() const
{
    if (isFloating())
    {
        return dockContainer()->floatingWidget()->isFullScreen();
    }
    else
    {
        return QFrame::isFullScreen();
    }
}

void QdfDockWidget::setAsCurrentTab()
{
    QDF_D(QdfDockWidget)
    if (d->dockArea && !isClosed())
    {
        d->dockArea->setCurrentDockWidget(this);
    }
}

bool QdfDockWidget::isTabbed() const
{
    QDF_D(const QdfDockWidget)
    return d->dockArea && (d->dockArea->openedDockWidgetsCount() > 1);
}

bool QdfDockWidget::isCurrentTab() const
{
    QDF_D(const QdfDockWidget)
    return d->dockArea && (d->dockArea->currentDockWidget() == this);
}

void QdfDockWidget::raise()
{
    if (isClosed())
    {
        return;
    }

    setAsCurrentTab();
    if (isInFloatingContainer())
    {
        auto floatingWindow = window();
        floatingWindow->raise();
        floatingWindow->activateWindow();
    }
}

void QdfDockWidget::setAutoHide(bool enable, SideBarLocation location)
{
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
    {
        return;
    }

    if (enable == autoHide())
    {
        return;
    }

    auto dockArea = dockAreaWidget();
    if (!enable)
    {
        dockArea->setAutoHide(false);
    }
    else
    {
        auto area = (SideBarLocation::SL_None == location) ? dockArea->calculateSideTabBarArea() : location;
        dockContainer()->createAndSetupAutoHideContainer(area, this);
    }
}

void QdfDockWidget::toggleAutoHide(SideBarLocation location)
{
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
    {
        return;
    }

    setAutoHide(!autoHide(), location);
}