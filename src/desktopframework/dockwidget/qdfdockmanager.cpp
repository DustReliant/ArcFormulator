#include "private/qdfdockmanager_p.h"
#include "qdfdockwidgethelper.h"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QSettings>
#include <QVariant>
#include <QWindow>
#include <QWindowStateChangeEvent>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockfocuscontroller.h>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdffloatingdockcontainer.h>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    #include "linux/qdffloatingwidgettitlebar.h"
    #include "qdfdockmanager.h"
#endif

QDF_USE_NAMESPACE

QdfDockManagerPrivate::QdfDockManagerPrivate()
{
}

void QdfDockManagerPrivate::loadStylesheet()
{
    QDF_Q(QdfDockManager)
    QString Result;
    QString FileName = ":ads/stylesheets/";
    FileName += QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting) ? "focus_highlighting"
                                                                                  : "default";
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    FileName += "_linux";
#endif
    FileName += ".css";
    QFile StyleSheetFile(FileName);
    StyleSheetFile.open(QIODevice::ReadOnly);
    QTextStream StyleSheetStream(&StyleSheetFile);
    Result = StyleSheetStream.readAll();
    StyleSheetFile.close();
    q->setStyleSheet(Result);
}

void QdfDockManagerPrivate::restoreDockWidgetsOpenState()
{
    for (auto &dockWidget: dockWidgetsMap)
    {
        if (dockWidget->property(internal::DirtyProperty).toBool())
        {
            if (dockWidget->autoHide())
            {
                dockWidget->autoHideDockContainer()->cleanupAndDelete();
            }
            dockWidget->flagAsUnassigned();
            Q_EMIT dockWidget->viewToggled(false);
        }
        else
        {
            dockWidget->toggleViewInternal(!dockWidget->property(internal::ClosedProperty).toBool());
        }
    }
}

void QdfDockManagerPrivate::restoreDockAreasIndices()
{
    QDF_Q(QdfDockManager)
    int Count = 0;
    for (auto &dockContainer: containers)
    {
        Count++;
        for (int i = 0; i < dockContainer->dockAreaCount(); ++i)
        {
            QdfDockAreaWidget *dockArea = dockContainer->dockArea(i);
            QString name = dockArea->property("currentDockWidget").toString();
            QdfDockWidget *dockWidget = nullptr;
            if (!name.isEmpty())
            {
                dockWidget = q->findDockWidget(name);
            }

            if (!dockWidget || dockWidget->isClosed())
            {
                int index = dockArea->indexOfFirstOpenDockWidget();
                if (index < 0)
                {
                    continue;
                }
                dockArea->setCurrentIndex(index);
            }
            else
            {
                dockArea->internalSetCurrentDockWidget(dockWidget);
            }
        }
    }
}

void QdfDockManagerPrivate::emitTopLevelEvents()
{
    for (auto &dockContainer: containers)
    {
        QdfDockWidget *topLevelDockWidget = dockContainer->topLevelDockWidget();
        if (topLevelDockWidget)
        {
            topLevelDockWidget->emitTopLevelChanged(true);
        }
        else
        {
            for (int i = 0; i < dockContainer->dockAreaCount(); ++i)
            {
                auto dockArea = dockContainer->dockArea(i);
                for (auto dockWidget: dockArea->dockWidgets())
                {
                    dockWidget->emitTopLevelChanged(false);
                }
            }
        }
    }
}

void QdfDockManagerPrivate::hideFloatingWidgets()
{
    for (auto &floatingWidget: floatingWidgets)
    {
        floatingWidget->hide();
    }
}
void QdfDockManagerPrivate::markDockWidgetsDirty()
{
    for (auto &dockWidget: dockWidgetsMap)
    {
        dockWidget->setProperty(internal::DirtyProperty, true);
    }
}

void QdfDockManagerPrivate::addActionToMenu(QAction *action, QMenu *menu, bool insertSorted)
{
    if (insertSorted)
    {
        auto actions = menu->actions();
        auto it = std::find_if(actions.begin(), actions.end(), [&action](const QAction *a) {
            return a->text().compare(action->text(), Qt::CaseInsensitive) > 0;
        });

        if (it == actions.end())
        {
            menu->addAction(action);
        }
        else
        {
            menu->insertAction(*it, action);
        }
    }
    else
    {
        menu->addAction(action);
    }
}

enum eStateFileVersion
{
    InitialVersion = 0,
    Version1 = 1,
    CurrentVersion = Version1
};

static QdfDockManager::ConfigFlags StaticConfigFlags = QdfDockManager::DefaultNonOpaqueConfig;
static QdfDockManager::AutoHideFlags StaticAutoHideConfigFlags;
static QString FloatingcontainersTitle;

QdfDockManager::QdfDockManager(QWidget *parent) : QdfDockContainerWidget(this, parent)
{
    QDF_INIT_PRIVATE(QdfDockManager)
    QDF_D(QdfDockManager)
    createRootSplitter();
    createSideTabBarWidgets();
    QMainWindow *MainWindow = qobject_cast<QMainWindow *>(parent);
    if (MainWindow)
    {
        MainWindow->setCentralWidget(this);
    }

    d->viewMenu = new QMenu(tr("Show View"), this);
    d->dockAreaOverlay = new QdfDockOverlay(this, QdfDockOverlay::ModeDockAreaOverlay);
    d->containerOverlay = new QdfDockOverlay(this, QdfDockOverlay::ModeContainerOverlay);
    d->containers.append(this);
    d->loadStylesheet();

    if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
    {
        d->focusController = new QdfDockFocusController(this);
    }

    window()->installEventFilter(this);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    connect(qApp, &QApplication::focusWindowChanged, [](QWindow *focusWindow) {
        if (focusWindow && focusWindow->isModal())
        {
            focusWindow->raise();
        }
    });
#endif
}

QdfDockManager::~QdfDockManager()
{
    std::vector<QdfDockAreaWidget *> areas;
    for (int i = 0; i != dockAreaCount(); ++i)
    {
        areas.push_back(dockArea(i));
    }
    for (auto area: areas)
    {
        for (auto widget: area->dockWidgets())
        {
            delete widget;
        }

        delete area;
    }

    QDF_D(QdfDockManager)
    for (auto &widget: d->floatingWidgets)
    {
        delete widget;
    }

    delete d;
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
bool QdfDockManager::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        for (auto &_window: floatingWidgets())
        {
            if (!_window->isVisible() || window()->isMinimized())
            {
                continue;
            }
            if (QGuiApplication::platformName() == QLatin1String("xcb"))
            {
                internal::xcb_update_prop(true, _window->window()->winId(), "_NET_WM_STATE",
                                          "_NET_WM_STATE_ABOVE", "_NET_WM_STATE_STAYS_ON_TOP");
            }
            else
            {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            }
        }
    }
    else if (event->type() == QEvent::WindowDeactivate)
    {
        for (auto &_window: floatingWidgets())
        {
            if (!_window->isVisible() || window()->isMinimized())
            {
                continue;
            }

            if (QGuiApplication::platformName() == QLatin1String("xcb"))
            {
                internal::xcb_update_prop(false, _window->window()->winId(), "_NET_WM_STATE",
                                          "_NET_WM_STATE_ABOVE", "_NET_WM_STATE_STAYS_ON_TOP");
            }
            else
            {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            }
            _window->raise();
        }
    }

    // Sync minimize with MainWindow
    if (event->type() == QEvent::WindowStateChange)
    {
        for (auto _window: floatingWidgets())
        {
            if (!_window->isVisible())
            {
                continue;
            }

            if (window()->isMinimized())
            {
                _window->showMinimized();
            }
            else
            {
                _window->setWindowState(_window->windowState() & (~Qt::WindowMinimized));
            }
        }
        if (!window()->isMinimized())
        {
            QApplication::setActiveWindow(window());
        }
    }
    return QdfDockContainerWidget::eventFilter(obj, e);
}
#else
bool QdfDockManager::eventFilter(QObject *obj, QEvent *event)
{
    QDF_D(QdfDockManager)
    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(event);
        if (ev->oldState().testFlag(Qt::WindowMinimized))
        {
            d->isLeavingMinimized = true;
            QMetaObject::invokeMethod(this, "endLeavingMinimizedState", Qt::QueuedConnection);
        }
    }
    return QdfDockContainerWidget::eventFilter(obj, event);
}
#endif

void QdfDockManager::endLeavingMinimizedState()
{
    QDF_D(QdfDockManager)
    d->isLeavingMinimized = false;
    this->activateWindow();
}

bool QdfDockManager::isLeavingMinimizedState() const
{
    QDF_D(const QdfDockManager)
    return d->isLeavingMinimized;
}

void QdfDockManager::registerFloatingWidget(QdfFloatingDockContainer *floatingWidget)
{
    QDF_D(QdfDockManager)
    d->floatingWidgets.append(floatingWidget);
    Q_EMIT floatingWidgetCreated(floatingWidget);
}

void QdfDockManager::removeFloatingWidget(QdfFloatingDockContainer *floatingWidget)
{
    QDF_D(QdfDockManager)
    d->floatingWidgets.removeAll(floatingWidget);
}

void QdfDockManager::registerDockContainer(QdfDockContainerWidget *dockContainer)
{
    QDF_D(QdfDockManager)
    d->containers.append(dockContainer);
}

void QdfDockManager::removeDockContainer(QdfDockContainerWidget *dockContainer)
{
    QDF_D(QdfDockManager)
    if (this != dockContainer)
    {
        d->containers.removeAll(dockContainer);
    }
}

QdfDockOverlay *QdfDockManager::containerOverlay() const
{
    QDF_D(const QdfDockManager)
    return d->containerOverlay;
}

QdfDockOverlay *QdfDockManager::dockAreaOverlay() const
{
    QDF_D(const QdfDockManager)
    return d->dockAreaOverlay;
}

const QList<QdfDockContainerWidget *> QdfDockManager::dockContainers() const
{
    QDF_D(const QdfDockManager)
    return d->containers;
}

const QList<QdfFloatingDockContainer *> QdfDockManager::floatingWidgets() const
{
    QDF_D(const QdfDockManager)
    return d->floatingWidgets;
}

unsigned int QdfDockManager::zOrderIndex() const
{
    return 0;
}

QdfFloatingDockContainer *QdfDockManager::addDockWidgetFloating(QdfDockWidget *Dockwidget)
{
    QDF_D(QdfDockManager)
    d->dockWidgetsMap.insert(Dockwidget->objectName(), Dockwidget);
    QdfDockAreaWidget *OldDockArea = Dockwidget->dockAreaWidget();
    if (OldDockArea)
    {
        OldDockArea->removeDockWidget(Dockwidget);
    }

    Dockwidget->setDockManager(this);
    QdfFloatingDockContainer *FloatingWidget = new QdfFloatingDockContainer(Dockwidget);
    FloatingWidget->resize(Dockwidget->size());
    if (isVisible())
    {
        FloatingWidget->show();
    }
    else
    {
        d->uninitializedFloatingWidgets.append(FloatingWidget);
    }
    Q_EMIT dockWidgetAdded(Dockwidget);
    return FloatingWidget;
}

void QdfDockManager::showEvent(QShowEvent *event)
{
    QDF_D(QdfDockManager)
    QdfDockContainerWidget::showEvent(event);
    restoreHiddenFloatingWidgets();
    if (d->uninitializedFloatingWidgets.empty())
    {
        return;
    }

    for (auto FloatingWidget: d->uninitializedFloatingWidgets)
    {
        if (FloatingWidget->dockContainer()->hasOpenDockAreas())
        {
            FloatingWidget->show();
        }
    }
    d->uninitializedFloatingWidgets.clear();
}

void QdfDockManager::restoreHiddenFloatingWidgets()
{
    QDF_D(QdfDockManager)
    if (d->hiddenFloatingWidgets.isEmpty())
    {
        return;
    }

    for (auto &floatingWidget: d->hiddenFloatingWidgets)
    {
        bool hasDockWidgetVisible = false;
        for (auto &dockWidget: floatingWidget->dockWidgets())
        {
            if (dockWidget->toggleViewAction()->isChecked())
            {
                dockWidget->toggleView(true);
                hasDockWidgetVisible = true;
            }
        }

        if (hasDockWidgetVisible)
        {
            floatingWidget->show();
        }
    }

    d->hiddenFloatingWidgets.clear();
}

QdfDockAreaWidget *QdfDockManager::addDockWidget(DockWidgetArea area, QdfDockWidget *dockWidget,
                                                 QdfDockAreaWidget *dockAreaWidget, int index)
{
    QDF_D(QdfDockManager)
    d->dockWidgetsMap.insert(dockWidget->objectName(), dockWidget);
    auto container = dockAreaWidget ? dockAreaWidget->dockContainer() : this;
    auto areaOfAddedDockWidget = container->addDockWidget(area, dockWidget, dockAreaWidget, index);
    Q_EMIT dockWidgetAdded(dockWidget);
    return areaOfAddedDockWidget;
}

QdfDockAreaWidget *QdfDockManager::addDockWidgetToContainer(DockWidgetArea area,
                                                            QdfDockWidget *dockWidget,
                                                            QdfDockContainerWidget *dockContainerWidget)
{
    QDF_D(QdfDockManager)
    d->dockWidgetsMap.insert(dockWidget->objectName(), dockWidget);
    auto AreaOfAddedDockWidget = dockContainerWidget->addDockWidget(area, dockWidget);
    Q_EMIT dockWidgetAdded(dockWidget);
    return AreaOfAddedDockWidget;
}

QdfAutoHideDockContainer *QdfDockManager::addAutoHideDockWidget(SideBarLocation area,
                                                                QdfDockWidget *dockWidget)
{
    return addAutoHideDockWidgetToContainer(area, dockWidget, this);
}

QdfAutoHideDockContainer *
QdfDockManager::addAutoHideDockWidgetToContainer(SideBarLocation area, QdfDockWidget *dockWidget,
                                                 QdfDockContainerWidget *dockContainerWidget)
{
    QDF_D(QdfDockManager)
    d->dockWidgetsMap.insert(dockWidget->objectName(), dockWidget);
    auto container = dockContainerWidget->createAndSetupAutoHideContainer(area, dockWidget);
    container->collapseView(true);

    Q_EMIT dockWidgetAdded(dockWidget);
    return container;
}

QdfDockAreaWidget *QdfDockManager::addDockWidgetTab(DockWidgetArea area, QdfDockWidget *dockWidget)
{
    QdfDockAreaWidget *areaWidget = lastAddedDockAreaWidget(area);
    if (areaWidget)
    {
        return addDockWidget(CenterDockWidgetArea, dockWidget, areaWidget);
    }
    else
    {
        return addDockWidget(area, dockWidget, nullptr);
    }
}

QdfDockAreaWidget *QdfDockManager::addDockWidgetTabToArea(QdfDockWidget *dockWidget,
                                                          QdfDockAreaWidget *dockAreaWidget, int index)
{
    return addDockWidget(CenterDockWidgetArea, dockWidget, dockAreaWidget, index);
}

QdfDockWidget *QdfDockManager::findDockWidget(const QString &ObjectName) const
{
    QDF_D(const QdfDockManager)
    return d->dockWidgetsMap.value(ObjectName, nullptr);
}

void QdfDockManager::removeDockWidget(QdfDockWidget *dockWidget)
{
    QDF_D(QdfDockManager)
    Q_EMIT dockWidgetAboutToBeRemoved(dockWidget);
    d->dockWidgetsMap.remove(dockWidget->objectName());
    QdfDockContainerWidget::removeDockWidget(dockWidget);
    dockWidget->setDockManager(nullptr);
    Q_EMIT dockWidgetRemoved(dockWidget);
}

QMap<QString, QdfDockWidget *> QdfDockManager::dockWidgetsMap() const
{
    QDF_D(const QdfDockManager)
    return d->dockWidgetsMap;
}

QdfDockWidget *QdfDockManager::centralWidget() const
{
    QDF_D(const QdfDockManager)
    return d->centralWidget;
}

QdfDockAreaWidget *QdfDockManager::setCentralWidget(QdfDockWidget *widget)
{
    QDF_D(QdfDockManager)
    if (!widget)
    {
        d->centralWidget = nullptr;
        return nullptr;
    }

    if (d->centralWidget)
    {
        qWarning("Setting a central widget not possible because there is already a central widget.");
        return nullptr;
    }

    if (!d->dockWidgetsMap.isEmpty())
    {
        qWarning("Setting a central widget not possible - the central widget need to be the first "
                 "dock widget that is added to the dock manager.");
        return nullptr;
    }

    widget->setFeature(QdfDockWidget::DockWidgetClosable, false);
    widget->setFeature(QdfDockWidget::DockWidgetMovable, false);
    widget->setFeature(QdfDockWidget::DockWidgetFloatable, false);
    widget->setFeature(QdfDockWidget::DockWidgetPinnable, false);
    d->centralWidget = widget;
    QdfDockAreaWidget *CentralArea = addDockWidget(CenterDockWidgetArea, widget);
    CentralArea->setDockAreaFlag(QdfDockAreaWidget::eDockAreaFlag::HideSingleWidgetTitleBar, true);
    return CentralArea;
}

QAction *QdfDockManager::addToggleViewActionToMenu(QAction *ToggleViewAction, const QString &Group,
                                                   const QIcon &GroupIcon)
{
    QDF_D(QdfDockManager)
    bool AlphabeticallySorted = (MenuAlphabeticallySorted == d->menuInsertionOrder);
    if (!Group.isEmpty())
    {
        QMenu *GroupMenu = d->viewMenuGroups.value(Group, 0);
        if (!GroupMenu)
        {
            GroupMenu = new QMenu(Group, this);
            GroupMenu->setIcon(GroupIcon);
            d->addActionToMenu(GroupMenu->menuAction(), d->viewMenu, AlphabeticallySorted);
            d->viewMenuGroups.insert(Group, GroupMenu);
        }
        else if (GroupMenu->icon().isNull() && !GroupIcon.isNull())
        {
            GroupMenu->setIcon(GroupIcon);
        }

        d->addActionToMenu(ToggleViewAction, GroupMenu, AlphabeticallySorted);
        return GroupMenu->menuAction();
    }
    else
    {
        d->addActionToMenu(ToggleViewAction, d->viewMenu, AlphabeticallySorted);
        return ToggleViewAction;
    }
}

QMenu *QdfDockManager::viewMenu() const
{
    QDF_D(const QdfDockManager)
    return d->viewMenu;
}

void QdfDockManager::setViewMenuInsertionOrder(ViewMenuInsertionOrder Order)
{
    QDF_D(QdfDockManager)
    d->menuInsertionOrder = Order;
}

//===========================================================================
bool QdfDockManager::isRestoringState() const
{
    QDF_D(const QdfDockManager)
    return d->restoringState;
}

int QdfDockManager::startDragDistance()
{
    return QApplication::startDragDistance() * 1.5;
}

QdfDockManager::ConfigFlags QdfDockManager::configFlags()
{
    return StaticConfigFlags;
}

QdfDockManager::AutoHideFlags QdfDockManager::autoHideConfigFlags()
{
    return StaticAutoHideConfigFlags;
}

void QdfDockManager::setConfigFlags(const ConfigFlags GsFlags)
{
    StaticConfigFlags = GsFlags;
}

void QdfDockManager::setAutoHideConfigFlags(const AutoHideFlags GsFlags)
{
    StaticAutoHideConfigFlags = GsFlags;
}

void QdfDockManager::setConfigFlag(ConfigFlag GsFlag, bool On)
{
    internal::setFlag(StaticConfigFlags, GsFlag, On);
}

void QdfDockManager::setAutoHideConfigFlag(AutoHideFlag GsFlag, bool On)
{
    internal::setFlag(StaticAutoHideConfigFlags, GsFlag, On);
}

bool QdfDockManager::testConfigFlag(ConfigFlag GsFlag)
{
    return configFlags().testFlag(GsFlag);
}

bool QdfDockManager::testAutoHideConfigFlag(AutoHideFlag GsFlag)
{
    return autoHideConfigFlags().testFlag(GsFlag);
}

void QdfDockManager::notifyWidgetOrAreaRelocation(QWidget *DroppedWidget)
{
    QDF_D(QdfDockManager)
    if (d->focusController)
    {
        d->focusController->notifyWidgetOrAreaRelocation(DroppedWidget);
    }
}

void QdfDockManager::notifyFloatingWidgetDrop(QdfFloatingDockContainer *FloatingWidget)
{
    QDF_D(QdfDockManager)
    if (d->focusController)
    {
        d->focusController->notifyFloatingWidgetDrop(FloatingWidget);
    }
}

void QdfDockManager::setDockWidgetFocused(QdfDockWidget *DockWidget)
{
    QDF_D(QdfDockManager)
    if (d->focusController)
    {
        d->focusController->setDockWidgetFocused(DockWidget);
    }
}

void QdfDockManager::hideManagerAndFloatingWidgets()
{
    QDF_D(QdfDockManager)
    hide();

    d->hiddenFloatingWidgets.clear();
    for (auto &floatingWidget: d->floatingWidgets)
    {
        if (floatingWidget->isVisible())
        {
            QList<QdfDockWidget *> VisibleWidgets;
            for (auto &dockWidget: floatingWidget->dockWidgets())
            {
                if (dockWidget->toggleViewAction()->isChecked())
                {
                    VisibleWidgets.push_back(dockWidget);
                }
            }

            d->hiddenFloatingWidgets.push_back(floatingWidget);
            floatingWidget->hide();
            for (auto &dockWidget: VisibleWidgets)
            {
                dockWidget->toggleViewAction()->setChecked(true);
            }
        }
    }
}

QdfDockWidget *QdfDockManager::focusedDockWidget() const
{
    QDF_D(const QdfDockManager)
    if (!d->focusController)
    {
        return nullptr;
    }
    else
    {
        return d->focusController->focusedDockWidget();
    }
}

QList<int> QdfDockManager::splitterSizes(QdfDockAreaWidget *ContainedArea) const
{
    if (ContainedArea)
    {
        auto Splitter = internal::findParent<QdfDockSplitter *>(ContainedArea);
        if (Splitter)
        {
            return Splitter->sizes();
        }
    }
    return QList<int>();
}

void QdfDockManager::setSplitterSizes(QdfDockAreaWidget *ContainedArea, const QList<int> &sizes)
{
    if (!ContainedArea)
    {
        return;
    }

    auto Splitter = internal::findParent<QdfDockSplitter *>(ContainedArea);
    if (Splitter && Splitter->count() == sizes.count())
    {
        Splitter->setSizes(sizes);
    }
}

void QdfDockManager::setFloatingContainersTitle(const QString &title)
{
    FloatingcontainersTitle = title;
}

QString QdfDockManager::floatingContainersTitle()
{
    if (FloatingcontainersTitle.isEmpty())
    {
        return qApp->applicationDisplayName();
    }

    return FloatingcontainersTitle;
}

QdfDockFocusController *QdfDockManager::dockFocusController() const
{
    QDF_D(const QdfDockManager)
    return d->focusController;
}