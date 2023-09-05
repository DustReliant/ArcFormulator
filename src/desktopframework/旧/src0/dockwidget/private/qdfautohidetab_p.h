#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDETAB_P_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDETAB_P_H

#include <QElapsedTimer>
#include <dockwidget/qdfautohidetab.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfDockContainerWidget;
class QdfAutoHideSideBar;

class QdfAutoHideTabPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfAutoHideTab)
    QdfAutoHideTabPrivate();
    void updateOrientation();
    QdfDockContainerWidget *dockContainer() const;
    void forwardEventToDockContainer(QEvent *event);

private:
    QdfDockWidget *dockWidget = nullptr;
    QdfAutoHideSideBar *sideBar = nullptr;
    Qt::Orientation orientation{Qt::Vertical};
    QElapsedTimer timeSinceHoverMousePress;
    QdfAutoHideTab::Orientation buttonOrientation{QdfAutoHideTab::Horizontal};
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDETAB_P_H