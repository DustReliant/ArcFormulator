#ifndef DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_P_H
#define DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_P_H

#include <QPointer>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfDockAreaWidget;
class QdfDockManager;
class QdfFloatingDockContainer;

class QdfDockFocusController;
class QdfDockFocusControllerPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockFocusController)

    QdfDockFocusControllerPrivate() = default;
    void updateDockWidgetFocus(QdfDockWidget *dockWidget);

private:
    QPointer<QdfDockWidget> focusedDockWidget = nullptr;
    QPointer<QdfDockAreaWidget> focusedArea = nullptr;
    QPointer<QdfDockWidget> oldFocusedDockWidget = nullptr;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QPointer<QdfFloatingDockContainer> floatingWidget = nullptr;
#endif
    QdfDockManager *dockManager = nullptr;
    bool forceFocusChangedSignal = false;
};

QDF_END_NAMESPACE

#endif// DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_P_H