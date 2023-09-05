#ifndef DESKTOPFRAMEWORK_QDFDOCKWIDGET_H
#define DESKTOPFRAMEWORK_QDFDOCKWIDGET_H

#include <QFrame>
#include <QToolBar>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidgetTab;
class QdfDockManager;
class QdfAutoHideTab;
class QdfDockContainerWidget;
class QdfFloatingDockContainer;
class QdfDockAreaWidget;
class QdfAutoHideDockContainer;

class QdfDockWidgetPrivate;
class QDF_EXPORT QdfDockWidget : public QFrame
{
    Q_OBJECT
public:
    enum DockWidgetFeature
    {
        DockWidgetClosable = 0x001,
        DockWidgetMovable = 0x002,
        DockWidgetFloatable = 0x004,
        DockWidgetDeleteOnClose = 0x008,
        CustomCloseHandling = 0x010,
        DockWidgetFocusable = 0x020,
        DockWidgetForceCloseWithArea = 0x040,
        NoTab = 0x080,
        DeleteContentOnClose = 0x100,
        DockWidgetPinnable = 0x200,
        DefaultDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable |
                                    DockWidgetFocusable | DockWidgetPinnable,
        AllDockWidgetFeatures =
                DefaultDockWidgetFeatures | DockWidgetDeleteOnClose | CustomCloseHandling,
        DockWidgetAlwaysCloseAndDelete = DockWidgetForceCloseWithArea | DockWidgetDeleteOnClose,
        NoDockWidgetFeatures = 0x000
    };
    Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)

    enum DockWidgetState
    {
        StateHidden,
        StateDocked,
        StateFloating
    };
    Q_ENUM(DockWidgetState)

    enum InsertMode
    {
        AutoScrollArea,
        ForceScrollArea,
        ForceNoScrollArea
    };
    Q_ENUM(InsertMode)

    enum MinimumSizeHintMode
    {
        MinimumSizeHintFromDockWidget,
        MinimumSizeHintFromContent,
        MinimumSizeHintFromDockWidgetMinimumSize,
        MinimumSizeHintFromContentMinimumSize,
    };
    Q_ENUM(MinimumSizeHintMode)

    enum ToggleViewActionMode
    {
        ActionModeToggle,
        ActionModeShow
    };
    Q_ENUM(ToggleViewActionMode)

public:
    QdfDockWidget(const QString &title, QWidget *parent = nullptr);
    virtual ~QdfDockWidget() override;

    void setWidget(QWidget *widget, InsertMode InsertMode = AutoScrollArea);
    using FactoryFunc = std::function<QWidget *(QWidget *)>;
    void setWidgetFactory(FactoryFunc createWidget, InsertMode InsertMode = AutoScrollArea);

    QWidget *takeWidget();
    QWidget *widget() const;
    QdfDockWidgetTab *tabWidget() const;
    void setFeatures(DockWidgetFeatures features);
    void setFeature(DockWidgetFeature feature, bool on);
    DockWidgetFeatures features() const;

    QdfDockManager *dockManager() const;

    QdfDockContainerWidget *dockContainer() const;
    QdfFloatingDockContainer *floatingDockContainer() const;
    QdfDockAreaWidget *dockAreaWidget() const;
    QdfAutoHideTab *sideTabWidget() const;
    void setSideTabWidget(QdfAutoHideTab *sideTab);
    bool autoHide() const;
    QdfAutoHideDockContainer *autoHideDockContainer() const;

    bool isFloating() const;
    bool isInFloatingContainer() const;
    bool isClosed() const;

    QAction *toggleViewAction() const;

    void setToggleViewActionMode(ToggleViewActionMode mode);
    MinimumSizeHintMode minimumSizeHintMode() const;
    void setMinimumSizeHintMode(MinimumSizeHintMode mode);

    bool isCentralWidget() const;
    void setIcon(const QIcon &icon);
    QIcon icon() const;

    QToolBar *toolBar() const;
    QToolBar *createDefaultToolBar();
    void setToolBar(QToolBar *toolBar);
    void setToolBarStyle(Qt::ToolButtonStyle style, DockWidgetState state);
    Qt::ToolButtonStyle toolBarStyle(DockWidgetState state) const;
    void setToolBarIconSize(const QSize &size, DockWidgetState state);
    QSize toolBarIconSize(DockWidgetState state) const;
    void setTitleBarActions(QList<QAction *> actions);
    virtual QList<QAction *> titleBarActions() const;
    virtual QSize minimumSizeHint() const override;

#ifndef QT_NO_TOOLTIP
    void setTabToolTip(const QString &text);
#endif
    bool isFullScreen() const;
    bool isTabbed() const;
    bool isCurrentTab() const;

public:
    virtual bool event(QEvent *event) override;

public Q_SLOTS:
    void toggleView(bool open = true);
    void setAsCurrentTab();
    void raise();
    void setFloating();
    void deleteDockWidget();
    void closeDockWidget();
    void showFullScreen();
    void showNormal();
    void setAutoHide(bool enable, SideBarLocation location = SideBarLocation::SL_None);
    void toggleAutoHide(SideBarLocation location = SideBarLocation::SL_None);


Q_SIGNALS:
    void viewToggled(bool open);
    void closed();
    void titleChanged(const QString &title);
    void topLevelChanged(bool topLevel);
    void closeRequested();
    void visibilityChanged(bool visible);
    void featuresChanged(QdfDockWidget::DockWidgetFeatures features);

protected:
    void setDockManager(QdfDockManager *dockManager);
    void setDockArea(QdfDockAreaWidget *dockArea);
    void setToggleViewActionChecked(bool checked);
    void flagAsUnassigned();
    static void emitTopLevelEventForWidget(QdfDockWidget *topLevelDockWidget, bool floating);
    void emitTopLevelChanged(bool floating);
    void setClosedState(bool closed);
    void toggleViewInternal(bool open);
    bool closeDockWidgetInternal(bool forceClose = false);

private Q_SLOTS:
    void setToolbarFloatingStyle(bool topLevel);

private:
    QDF_DECLARE_PRIVATE(QdfDockWidget)
    Q_DISABLE_COPY(QdfDockWidget)

    friend class QdfDockManagerPrivate;
    friend class QdfDockManager;
    friend class QdfFloatingDockContainer;
    friend class QdfFloatingDockContainerPrivate;
    friend class QdfDockAreaTitleBar;
    friend class QdfDockAreaTitleBarPrivate;
    friend class QdfDockContainerWidget;
    friend class QdfDockContainerWidgetPrivate;
    friend class QdfDockAreaWidget;
    friend class QdfDockAreaWidgetPrivate;
    friend class QdfAutoHideDockContainer;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKWIDGET_H