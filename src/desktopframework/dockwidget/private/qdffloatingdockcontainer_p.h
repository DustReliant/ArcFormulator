#ifndef DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_P_H
#define DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_P_H

#include "../qdfdockwidgethelper.h"
#include <QPoint>
#include <QPointer>
#include <QWidget>
#include <dockwidget/qdfdockmanager.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

static unsigned int zOrderCounter = 0;

class QdfDockContainerWidget;
class QdfFloatingDockContainer;
class QdfDockAreaWidget;

class QdfFloatingDockContainerPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfFloatingDockContainer)
public:
    QdfFloatingDockContainerPrivate() = default;
    void titleMouseReleaseEvent();
    void updateDropOverlays(const QPoint &globalPos);
    static bool testConfigFlag(QdfDockManager::ConfigFlag flag);
    bool isState(DragState id) const;
    void setState(DragState id);
    void setWindowTitle(const QString &text);
    void reflectCurrentWidget(QdfDockWidget *currentWidget);
    void handleEscapeKey();
    static QString floatingContainersTitle();

private:
    QdfDockContainerWidget *dockContainer;
    unsigned int zOrderIndex = ++zOrderCounter;
    QPointer<QdfDockManager> dockManager;
    DragState draggingState = DragState::DS_Inactive;
    QPoint dragStartMousePosition;
    QdfDockContainerWidget *dropContainer = nullptr;
    QdfDockAreaWidget *singleDockArea = nullptr;
    QPoint dragStartPos;
    bool hiding = false;
    bool autoHideChildren = true;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QWidget *mouseEventHandler = nullptr;
    QdfFloatingWidgetTitleBar *titleBar = nullptr;
    bool isResizing = false;
#endif
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_P_H