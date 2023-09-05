#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_P_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_P_H

#include <QBoxLayout>
#include <QPointer>
#include <QSize>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfResizeHandle;
class QdfDockAreaWidget;
class QdfDockWidget;
class QdfAutoHideDockContainer;
class QdfAutoHideTab;

class QdfAutoHideDockContainerPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfAutoHideDockContainer)

public:
    QdfAutoHideDockContainerPrivate();
    ~QdfAutoHideDockContainerPrivate();
    DockWidgetArea getDockWidgetArea(SideBarLocation area);
    void updateResizeHandleSizeLimitMax();
    bool isHorizontal() const;
    void forwardEventToDockContainer(QEvent *event);

private:
    QdfDockAreaWidget *dockArea;
    QdfDockWidget *dockWidget;
    SideBarLocation sideTabBarArea;
    QBoxLayout *pLayout;
    QdfResizeHandle *resizeHandle;
    QSize size;
    QPointer<QdfAutoHideTab> sideTab;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_P_H