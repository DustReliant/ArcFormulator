#include "qdfresizehandle.h"
#include "private/qdfresizehandle_p.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPointer>
#include <QRubberBand>
#include <QStyle>
#include <QStyleOption>

QDF_USE_NAMESPACE

QdfResizeHandlePrivate::QdfResizeHandlePrivate()
{
}

int QdfResizeHandlePrivate::pick(const QPoint &pos) const
{
    QDF_Q(const QdfResizeHandle)
    return q->orientation() == Qt::Horizontal ? pos.x() : pos.y();
}
bool QdfResizeHandlePrivate::isHorizontal() const
{
    QDF_Q(const QdfResizeHandle)
    return q->orientation() == Qt::Horizontal;
}


void QdfResizeHandlePrivate::setRubberBand(int Pos)
{
    if (!rubberBand)
    {
        rubberBand = new QRubberBand(QRubberBand::Line, target->parentWidget());
    }

    QDF_Q(QdfResizeHandle)
    auto Geometry = q->geometry();
    auto TopLeft = q->mapTo(target->parentWidget(), Geometry.topLeft());
    switch (handlePosition)
    {
        case Qt::LeftEdge:
        case Qt::RightEdge:
            TopLeft.rx() += Pos;
            break;
        case Qt::TopEdge:
        case Qt::BottomEdge:
            TopLeft.ry() += Pos;
            break;
    }

    Geometry.moveTopLeft(TopLeft);
    rubberBand->setGeometry(Geometry);
    rubberBand->show();
}


void QdfResizeHandlePrivate::doResizing(QMouseEvent *event, bool forceResize)
{
    QDF_Q(QdfResizeHandle)
    int pos = pick(event->pos()) - mouseOffset;
    auto OldGeometry = target->geometry();
    auto NewGeometry = OldGeometry;
    switch (handlePosition)
    {
        case Qt::LeftEdge:
            {
                NewGeometry.adjust(pos, 0, 0, 0);
                int Size = qBound(minSize, NewGeometry.width(), maxSize);
                pos += (NewGeometry.width() - Size);
                NewGeometry.setWidth(Size);
                NewGeometry.moveTopRight(OldGeometry.topRight());
            }
            break;


        case Qt::RightEdge:
            {
                NewGeometry.adjust(0, 0, pos, 0);
                int Size = qBound(minSize, NewGeometry.width(), maxSize);
                pos -= (NewGeometry.width() - Size);
                NewGeometry.setWidth(Size);
            }
            break;

        case Qt::TopEdge:
            {
                NewGeometry.adjust(0, pos, 0, 0);
                int Size = qBound(minSize, NewGeometry.height(), maxSize);
                pos += (NewGeometry.height() - Size);
                NewGeometry.setHeight(Size);
                NewGeometry.moveBottomLeft(OldGeometry.bottomLeft());
            }
            break;

        case Qt::BottomEdge:
            {
                NewGeometry.adjust(0, 0, 0, pos);
                int Size = qBound(minSize, NewGeometry.height(), maxSize);
                pos -= (NewGeometry.height() - Size);
                NewGeometry.setHeight(Size);
            }
            break;
    }

    if (q->opaqueResize() || forceResize)
    {
        target->setGeometry(NewGeometry);
    }
    else
    {
        setRubberBand(pos);
    }
}


QdfResizeHandle::QdfResizeHandle(Qt::Edge handlePosition, QWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfResizeHandle)
    QDF_D(QdfResizeHandle)
    d->target = parent;
    setMinResizeSize(48);
    setHandlePosition(handlePosition);
}


QdfResizeHandle::~QdfResizeHandle()
{
    QDF_FINI_PRIVATE()
}


void QdfResizeHandle::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfResizeHandle)
    if (!(event->buttons() & Qt::LeftButton))
    {
        return;
    }

    d->doResizing(event);
}


void QdfResizeHandle::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfResizeHandle)
    if (event->button() == Qt::LeftButton)
    {
        d->mouseOffset = d->pick(event->pos());
        d->pressed = true;
        update();
    }
}


void QdfResizeHandle::mouseReleaseEvent(QMouseEvent *event)
{
    QDF_D(QdfResizeHandle)
    if (!opaqueResize() && event->button() == Qt::LeftButton)
    {
        if (d->rubberBand)
        {
            d->rubberBand->deleteLater();
        }
        d->doResizing(event, true);
    }
    if (event->button() == Qt::LeftButton)
    {
        d->pressed = false;
        update();
    }
}

void QdfResizeHandle::setHandlePosition(Qt::Edge handlePosition)
{
    QDF_D(QdfResizeHandle)
    d->handlePosition = handlePosition;
    switch (d->handlePosition)
    {
        case Qt::LeftEdge:
        case Qt::RightEdge:
            setCursor(Qt::SizeHorCursor);
            break;

        case Qt::TopEdge:
        case Qt::BottomEdge:
            setCursor(Qt::SizeVerCursor);
            break;
    }

    setMaxResizeSize(d->isHorizontal() ? parentWidget()->height() : parentWidget()->width());
    if (!d->isHorizontal())
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    else
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

Qt::Edge QdfResizeHandle::handlePosition() const
{
    QDF_D(const QdfResizeHandle)
    return d->handlePosition;
}

Qt::Orientation QdfResizeHandle::orientation() const
{
    QDF_D(const QdfResizeHandle)
    switch (d->handlePosition)
    {
        case Qt::LeftEdge:
        case Qt::RightEdge:
            return Qt::Horizontal;

        case Qt::TopEdge:
        case Qt::BottomEdge:
            return Qt::Vertical;
    }

    return Qt::Horizontal;
}

QSize QdfResizeHandle::sizeHint() const
{
    QDF_D(const QdfResizeHandle)
    QSize Result;
    switch (d->handlePosition)
    {
        case Qt::LeftEdge:
        case Qt::RightEdge:
            Result = QSize(d->handleWidth, d->target->height());
            break;

        case Qt::TopEdge:
        case Qt::BottomEdge:
            Result = QSize(d->target->width(), d->handleWidth);
            break;
    }

    return Result;
}

bool QdfResizeHandle::isResizing() const
{
    QDF_D(const QdfResizeHandle)
    return d->pressed;
}

void QdfResizeHandle::setMinResizeSize(int size)
{
    QDF_D(QdfResizeHandle)
    d->minSize = size;
}

void QdfResizeHandle::setMaxResizeSize(int size)
{
    QDF_D(QdfResizeHandle)
    d->maxSize = size;
}

void QdfResizeHandle::setOpaqueResize(bool opaque)
{
    QDF_D(QdfResizeHandle)
    d->opaqueResize = opaque;
}

bool QdfResizeHandle::opaqueResize() const
{
    QDF_D(const QdfResizeHandle)
    return d->opaqueResize;
}