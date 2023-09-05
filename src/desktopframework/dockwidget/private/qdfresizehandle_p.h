#ifndef DESKTOPFRAMEWORK_QDFRESIZEHANDLE_P_H
#define DESKTOPFRAMEWORK_QDFRESIZEHANDLE_P_H

#include <QPointer>
#include <QRubberBand>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfResizeHandle;
class QdfResizeHandlePrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfResizeHandle)

public:
    QdfResizeHandlePrivate();
    int pick(const QPoint &pos) const;
    bool isHorizontal() const;
    void setRubberBand(int pos);
    void doResizing(QMouseEvent *event, bool forceResize = false);

    Qt::Edge handlePosition = Qt::LeftEdge;
    QWidget *target = nullptr;
    int mouseOffset = 0;
    bool pressed = false;
    int minSize = 0;
    int maxSize = 1;
    QPointer<QRubberBand> rubberBand;
    bool opaqueResize = false;
    int handleWidth = 4;

private:
    Q_DISABLE_COPY(QdfResizeHandlePrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRESIZEHANDLE_P_H