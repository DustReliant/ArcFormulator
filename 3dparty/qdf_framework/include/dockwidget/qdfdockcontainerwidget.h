#ifndef DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_H
#define DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_H

#include "qdfdockwidget.h"
#include <QFrame>
#include <QSplitter>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockManager;
class QdfDockAreaWidget;
class QdfFloatingDockContainer;
class QdfAutoHideSideBar;
class QdfAutoHideDockContainer;

class QdfDockContainerWidgetPrivate;
class QDF_EXPORT QdfDockContainerWidget : public QFrame
{
    Q_OBJECT
public:
    explicit QdfDockContainerWidget(QdfDockManager *manager, QWidget *parent = nullptr);
    virtual ~QdfDockContainerWidget() override;

    QdfDockAreaWidget *addDockWidget(DockWidgetArea area, QdfDockWidget *dockwidget,
                                     QdfDockAreaWidget *dockAreaWidget = nullptr, int index = -1);
    void removeDockWidget(QdfDockWidget *dockwidget);
    virtual unsigned int zOrderIndex() const;
    bool isInFrontOf(QdfDockContainerWidget *other) const;
    QdfDockAreaWidget *dockAreaAt(const QPoint &pos) const;
    QdfDockAreaWidget *dockArea(int index) const;
    QList<QdfDockAreaWidget *> openedDockAreas() const;
    QList<QdfDockWidget *> openedDockWidgets() const;
    bool hasOpenDockAreas() const;
    bool hasTopLevelDockWidget() const;
    int dockAreaCount() const;
    int visibleDockAreaCount() const;
    bool isFloating() const;
    void dumpLayout();
    QdfDockWidget::DockWidgetFeatures features() const;
    QdfFloatingDockContainer *floatingWidget() const;
    void closeOtherAreas(QdfDockAreaWidget *keepOpenArea);
    QdfAutoHideSideBar *sideTabBar(SideBarLocation area) const;
    QList<QdfAutoHideDockContainer *> autoHideWidgets() const;
    QRect contentRect() const;
    QRect contentRectGlobal() const;
    QdfDockManager *dockManager() const;

Q_SIGNALS:
    void dockAreasAdded();
    void autoHideWidgetCreated(QdfAutoHideDockContainer *autoHideWidget);
    void dockAreasRemoved();
    void dockAreaViewToggled(QdfDockAreaWidget *dockArea, bool open);

protected:
    virtual bool event(QEvent *event) override;
    QSplitter *rootSplitter() const;
    QdfAutoHideDockContainer *createAndSetupAutoHideContainer(SideBarLocation area,
                                                              QdfDockWidget *DockWidget);
    void createRootSplitter();
    void createSideTabBarWidgets();
    void dropFloatingWidget(QdfFloatingDockContainer *FloatingWidget, const QPoint &TargetPos);
    void dropWidget(QWidget *Widget, DockWidgetArea DropArea, QdfDockAreaWidget *TargetAreaWidget);
    void addDockArea(QdfDockAreaWidget *DockAreaWidget, DockWidgetArea area = CenterDockWidgetArea);
    void removeDockArea(QdfDockAreaWidget *area);
    QdfDockAreaWidget *lastAddedDockAreaWidget(DockWidgetArea area) const;
    QdfDockWidget *topLevelDockWidget() const;
    QdfDockAreaWidget *topLevelDockArea() const;
    QList<QdfDockWidget *> dockWidgets() const;
    void updateSplitterHandles(QSplitter *splitter);
    void registerAutoHideWidget(QdfAutoHideDockContainer *AutoHideWidget);
    void removeAutoHideWidget(QdfAutoHideDockContainer *AutoHideWidget);
    void handleAutoHideWidgetEvent(QEvent *e, QWidget *w);

private:
    QDF_DECLARE_PRIVATE(QdfDockContainerWidget)
    Q_DISABLE_COPY(QdfDockContainerWidget)
    friend class QdfAutoHideDockContainerPrivate;
    friend class QdfDockManagerPrivate;
    friend class QdfDockManager;
    friend class QdfFloatingDragPreview;
    friend class QdfFloatingDockContainerPrivate;
    friend class QdfFloatingDockContainer;
    friend class QdfDockWidget;
    friend class QdfDockWidgetPrivate;
    friend class QdfAutoHideTab;
    friend class QdfAutoHideTabPrivate;
    friend class QdfDockAreaWidget;
    friend class QdfAutoHideSideBar;
    friend class QdfAutoHideDockContainer;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_H