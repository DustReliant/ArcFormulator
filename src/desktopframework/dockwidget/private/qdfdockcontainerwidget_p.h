#ifndef DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_P_H
#define DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_P_H

#include <QGridLayout>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QSplitter>
#include <QTimer>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

enum DropMode
{
    DropModeIntoArea,
    DropModeIntoContainer,
    DropModeInvalid
};

class QdfDockSplitter;
class QdfAutoHideTab;
class QdfAutoHideSideBar;
class QdfDockAreaWidget;
class QdfFloatingDockContainer;
class QdfDockWidget;
class QdfAutoHideDockContainer;
class QdfDockManager;

class QdfDockContainerWidgetPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockContainerWidget)

    QdfDockContainerWidgetPrivate();

    QdfDockAreaWidget *addDockWidgetToContainer(DockWidgetArea area, QdfDockWidget *dockwidget);
    QdfDockAreaWidget *addDockWidgetToDockArea(DockWidgetArea area, QdfDockWidget *dockwidget,
                                               QdfDockAreaWidget *targetDockArea, int index = -1);

    void addDockArea(QdfDockAreaWidget *newDockWidget, DockWidgetArea area = CenterDockWidgetArea);
    void dropIntoContainer(QdfFloatingDockContainer *floatingWidget, DockWidgetArea area);
    void dropIntoSection(QdfFloatingDockContainer *floatingWidget, QdfDockAreaWidget *targetArea,
                         DockWidgetArea area);
    void moveToNewSection(QWidget *widget, QdfDockAreaWidget *targetArea, DockWidgetArea area);
    void moveToContainer(QWidget *widget, DockWidgetArea area);
    void dropIntoCenterOfSection(QdfFloatingDockContainer *floatingWidget,
                                 QdfDockAreaWidget *targetArea);
    void moveIntoCenterOfSection(QWidget *widget, QdfDockAreaWidget *targetArea);
    void addDockAreasToList(const QList<QdfDockAreaWidget *> newDockAreas);
    void appendDockAreas(const QList<QdfDockAreaWidget *> newDockAreas);
    void dumpRecursive(int level, QWidget *widget);

    DropMode getDropMode(const QPoint &targetPos);

    void initVisibleDockAreaCount();
    int &visibleDockAreaCount();
    void onVisibleDockAreaCountChanged();
    void emitDockAreasRemoved();
    void emitDockAreasAdded();

    QdfDockSplitter *newSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);

    void adjustSplitterSizesOnInsertion(QSplitter *splitter, qreal lastRatio = 1.0);
    void updateSplitterHandles(QSplitter *splitter);
    bool widgetResizesWithContainer(QWidget *widget);
    void onDockAreaViewToggled(bool visible);

private:
    QPointer<QdfDockManager> dockManager;
    unsigned int zOrderIndex = 0;
    QList<QdfDockAreaWidget *> dockAreas;
    QList<QdfAutoHideDockContainer *> autoHideWidgets;
    QMap<SideBarLocation, QdfAutoHideSideBar *> sideTabBarWidgets;
    QGridLayout *pLayout = nullptr;
    QSplitter *rootSplitter = nullptr;
    bool isFloating = false;
    QdfDockAreaWidget *lastAddedAreaCache[5];
    int VisibleDockAreaCount = -1;
    QdfDockAreaWidget *topLevelDockArea = nullptr;
    QTimer delayedAutoHideTimer;
    QdfAutoHideTab *delayedAutoHideTab;
    bool delayedAutoHideShow = false;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKCONTAINERWIDGET_P_H