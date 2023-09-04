#ifndef DESKTOPFRAMEWORK_QDFDOCKMANAGER_H
#define DESKTOPFRAMEWORK_QDFDOCKMANAGER_H

#include "qdfdockcontainerwidget.h"
#include <QIcon>
#include <QMenu>
#include <QSettings>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockOverlay;
class QdfDockFocusController;
class QdfDockManagerPrivate;
class QDF_EXPORT QdfDockManager : public QdfDockContainerWidget
{
    Q_OBJECT
public:
    enum ViewMenuInsertionOrder
    {
        MenuSortedByInsertion,
        MenuAlphabeticallySorted
    };

    enum ConfigFlag
    {
        ActiveTabHasCloseButton = 0x0001,
        DockAreaHasCloseButton = 0x0002,
        DockAreaCloseButtonClosesTab = 0x0004,
        OpaqueSplitterResize = 0x0008,
        XmlAutoFormattingEnabled = 0x0010,
        XmlCompressionEnabled = 0x0020,
        TabCloseButtonIsToolButton = 0x0040,
        AllTabsHaveCloseButton = 0x0080,
        RetainTabSizeWhenCloseButtonHidden = 0x0100,
        DragPreviewIsDynamic = 0x0400,
        DragPreviewShowsContentPixmap = 0x0800,
        DragPreviewHasWindowFrame = 0x1000,
        AlwaysShowTabs = 0x2000,
        DockAreaHasUndockButton = 0x4000,
        DockAreaHasTabsMenuButton = 0x8000,
        DockAreaHideDisabledButtons = 0x10000,
        DockAreaDynamicTabsMenuButtonVisibility = 0x20000,
        FloatingContainerHasWidgetTitle = 0x40000,
        FloatingContainerHasWidgetIcon = 0x80000,
        HideSingleCentralWidgetTitleBar = 0x100000,
        FocusHighlighting = 0x200000,
        EqualSplitOnInsertion = 0x400000,
        FloatingContainerForceNativeTitleBar = 0x800000,
        FloatingContainerForceQWidgetTitleBar = 0x1000000,
        MiddleMouseButtonClosesTab = 0x2000000,
        DefaultDockAreaButtons = DockAreaHasCloseButton | DockAreaHasUndockButton | DockAreaHasTabsMenuButton,
        DefaultBaseConfig = DefaultDockAreaButtons | ActiveTabHasCloseButton | XmlCompressionEnabled | FloatingContainerHasWidgetTitle,
        DefaultOpaqueConfig = DefaultBaseConfig | OpaqueSplitterResize | DragPreviewShowsContentPixmap,
        DefaultNonOpaqueConfig = DefaultBaseConfig | DragPreviewShowsContentPixmap,
        NonOpaqueWithWindowFrame = DefaultNonOpaqueConfig | DragPreviewHasWindowFrame
    };
    Q_DECLARE_FLAGS(ConfigFlags, ConfigFlag)

    enum AutoHideFlag
    {
        AutoHideFeatureEnabled = 0x01,
        DockAreaHasAutoHideButton = 0x02,
        AutoHideButtonTogglesArea = 0x04,
        AutoHideButtonCheckable = 0x08,
        AutoHideSideBarsIconOnly = 0x10,
        AutoHideShowOnMouseOver = 0x20,
        AutoHideCloseButtonCollapsesDock = 0x40,
        DefaultAutoHideConfig = AutoHideFeatureEnabled | DockAreaHasAutoHideButton
    };
    Q_DECLARE_FLAGS(AutoHideFlags, AutoHideFlag)

public:
    QdfDockManager(QWidget *parent = nullptr);
    virtual ~QdfDockManager() override;

    static ConfigFlags configFlags();
    static AutoHideFlags autoHideConfigFlags();
    static void setConfigFlags(const ConfigFlags flags);
    static void setAutoHideConfigFlags(const AutoHideFlags flags);
    static void setConfigFlag(ConfigFlag flag, bool on = true);
    static void setAutoHideConfigFlag(AutoHideFlag flag, bool on = true);
    static bool testConfigFlag(ConfigFlag flag);
    static bool testAutoHideConfigFlag(AutoHideFlag flag);
    static void setFloatingContainersTitle(const QString &title);
    static QString floatingContainersTitle();
    static int startDragDistance();

    template<typename T>
    static void setWidgetFocus(T t)
    {
        if (!QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
        {
            return;
        }

        t->setFocus(Qt::OtherFocusReason);
    }

    QdfDockAreaWidget *addDockWidget(DockWidgetArea area, QdfDockWidget *dockWidget,
                                     QdfDockAreaWidget *dockAreaWidget = nullptr, int index = -1);
    QdfDockAreaWidget *addDockWidgetToContainer(DockWidgetArea area, QdfDockWidget *dockWidget,
                                                QdfDockContainerWidget *DockContainerWidget);

    QdfAutoHideDockContainer *addAutoHideDockWidget(SideBarLocation location, QdfDockWidget *dockWidget);
    QdfAutoHideDockContainer *addAutoHideDockWidgetToContainer(SideBarLocation location,
                                                               QdfDockWidget *dockWidget, QdfDockContainerWidget *DockContainerWidget);

    QdfDockAreaWidget *addDockWidgetTab(DockWidgetArea area,
                                        QdfDockWidget *dockWidget);
    QdfDockAreaWidget *addDockWidgetTabToArea(QdfDockWidget *dockWidget,
                                              QdfDockAreaWidget *dockAreaWidget, int index = -1);
    QdfDockAreaWidget *setCentralWidget(QdfDockWidget *widget);

    QdfFloatingDockContainer *addDockWidgetFloating(QdfDockWidget *dockWidget);
    QdfDockWidget *findDockWidget(const QString &objectName) const;
    void removeDockWidget(QdfDockWidget *dockWidget);
    QMap<QString, QdfDockWidget *> dockWidgetsMap() const;
    const QList<QdfDockContainerWidget *> dockContainers() const;
    const QList<QdfFloatingDockContainer *> floatingWidgets() const;
    unsigned int zOrderIndex() const override;

    QdfDockWidget *centralWidget() const;
    QdfDockWidget *focusedDockWidget() const;

    QAction *addToggleViewActionToMenu(QAction *toggleViewAction,
                                       const QString &group = QString(), const QIcon &groupIcon = QIcon());
    QMenu *viewMenu() const;
    void setViewMenuInsertionOrder(ViewMenuInsertionOrder order);
    bool isRestoringState() const;
    bool isLeavingMinimizedState() const;


    bool eventFilter(QObject *watched, QEvent *event) override;
    QList<int> splitterSizes(QdfDockAreaWidget *containedArea) const;
    void setSplitterSizes(QdfDockAreaWidget *containedArea, const QList<int> &sizes);


public Q_SLOTS:
    void setDockWidgetFocused(QdfDockWidget *dockWidget);
    void hideManagerAndFloatingWidgets();
    void endLeavingMinimizedState();

Q_SIGNALS:
    void perspectiveListChanged();
    void perspectiveListLoaded();
    void perspectivesRemoved();
    void restoringState();
    void stateRestored();

    void openingPerspective(const QString &perspectiveName);
    void perspectiveOpened(const QString &perspectiveName);
    void floatingWidgetCreated(QdfFloatingDockContainer *floatingWidget);
    void dockAreaCreated(QdfDockAreaWidget *dockArea);
    void dockWidgetAdded(QdfDockWidget *dockWidget);
    void dockWidgetAboutToBeRemoved(QdfDockWidget *dockWidget);
    void dockWidgetRemoved(QdfDockWidget *dockWidget);
    void focusedDockWidgetChanged(QdfDockWidget *old, QdfDockWidget *now);

protected:
    void registerFloatingWidget(QdfFloatingDockContainer *floatingWidget);
    void removeFloatingWidget(QdfFloatingDockContainer *floatingWidget);
    void registerDockContainer(QdfDockContainerWidget *dockContainer);
    void removeDockContainer(QdfDockContainerWidget *dockContainer);

    QdfDockOverlay *containerOverlay() const;
    QdfDockOverlay *dockAreaOverlay() const;

    void notifyWidgetOrAreaRelocation(QWidget *relocatedWidget);
    void notifyFloatingWidgetDrop(QdfFloatingDockContainer *floatingWidget);
    virtual void showEvent(QShowEvent *event) override;
    QdfDockFocusController *dockFocusController() const;
    void restoreHiddenFloatingWidgets();

private:
    QDF_DECLARE_PRIVATE(QdfDockManager)
    Q_DISABLE_COPY(QdfDockManager)
    friend class QdfFloatingDragPreviewPrivate;
    friend class QdfFloatingDragPreview;
    friend class QdfFloatingDockContainerPrivate;
    friend class QdfFloatingDockContainer;
    friend class QdfDockWidgetTab;
    friend class QdfDockWidgetTabPrivate;
    friend class QdfDockAreaTitleBar;
    friend class QdfDockContainerWidgetPrivate;
    friend class QdfDockContainerWidget;
    friend class QdfAutoHideSideBar;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKMANAGER_H