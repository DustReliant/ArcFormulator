#ifndef DESKTOPFRAMEWORK_QDFFLOATINGWIDGET_H
#define DESKTOPFRAMEWORK_QDFFLOATINGWIDGET_H

#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfFloatingWidget
{
public:
    virtual ~QdfFloatingWidget() = default;
    virtual void startFloating(const QPoint &start, const QSize &size, DragState state, QWidget *mouseEventHandler) = 0;
    virtual void moveFloating() = 0;
    virtual void finishDragging() = 0;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGWIDGET_H