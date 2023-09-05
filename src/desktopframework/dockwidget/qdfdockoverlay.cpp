#include "qdfdockoverlaycross.h"
#include <QCursor>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWidget>
#include <dockwidget/qdfdockareatitlebar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockoverlay.h>

QDF_BEGIN_NAMESPACE

class QdfDockOverlayPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockOverlay)
    QdfDockOverlayPrivate() = default;

private:
    DockWidgetAreas allowedAreas = InvalidDockWidgetArea;
    QdfDockOverlayCross *cross;
    QPointer<QWidget> targetWidget;
    DockWidgetArea lastLocation = InvalidDockWidgetArea;
    bool dropPreviewEnabled = true;
    QdfDockOverlay::Mode mode = QdfDockOverlay::ModeDockAreaOverlay;
    QRect dropAreaRect;

private:
    Q_DISABLE_COPY(QdfDockOverlayPrivate)
};

QdfDockOverlay::QdfDockOverlay(QWidget *parent, Mode Mode) : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockOverlay)
    QDF_D(QdfDockOverlay)
    d->mode = Mode;
    d->cross = new QdfDockOverlayCross(this);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);
#else
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
    setWindowOpacity(1);
    setWindowTitle("DockOverlay");
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    d->cross->setVisible(false);
    setVisible(false);
}

QdfDockOverlay::~QdfDockOverlay()
{
    QDF_FINI_PRIVATE()
}

void QdfDockOverlay::setAllowedAreas(DockWidgetAreas areas)
{
    QDF_D(QdfDockOverlay)
    if (areas == d->allowedAreas)
    {
        return;
    }
    d->allowedAreas = areas;
    d->cross->reset();
}

DockWidgetAreas QdfDockOverlay::allowedAreas() const
{
    QDF_D(const QdfDockOverlay)
    return d->allowedAreas;
}

DockWidgetArea QdfDockOverlay::dropAreaUnderCursor() const
{
    QDF_D(const QdfDockOverlay)
    DockWidgetArea result = d->cross->cursorLocation();
    if (result != InvalidDockWidgetArea)
    {
        return result;
    }

    QdfDockAreaWidget *dockArea = qobject_cast<QdfDockAreaWidget *>(d->targetWidget.data());
    if (!dockArea)
    {
        return result;
    }

    if (dockArea->allowedAreas().testFlag(CenterDockWidgetArea) && !dockArea->titleBar()->isHidden() &&
        dockArea->titleBarGeometry().contains(dockArea->mapFromGlobal(QCursor::pos())))
    {
        return CenterDockWidgetArea;
    }

    return result;
}

DockWidgetArea QdfDockOverlay::visibleDropAreaUnderCursor() const
{
    QDF_D(const QdfDockOverlay)
    if (isHidden() || !d->dropPreviewEnabled)
    {
        return InvalidDockWidgetArea;
    }
    else
    {
        return dropAreaUnderCursor();
    }
}

DockWidgetArea QdfDockOverlay::showOverlay(QWidget *target)
{
    QDF_D(QdfDockOverlay)
    if (d->targetWidget == target)
    {
        DockWidgetArea da = dropAreaUnderCursor();
        if (da != d->lastLocation)
        {
            repaint();
            d->lastLocation = da;
        }
        return da;
    }

    d->targetWidget = target;
    d->lastLocation = InvalidDockWidgetArea;

    hide();
    resize(target->size());
    QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
    move(TopLeft);
    show();
    d->cross->updatePosition();
    d->cross->updateOverlayIcons();
    return dropAreaUnderCursor();
}

void QdfDockOverlay::hideOverlay()
{
    QDF_D(QdfDockOverlay)
    hide();
    d->targetWidget.clear();
    d->lastLocation = InvalidDockWidgetArea;
    d->dropAreaRect = QRect();
}

void QdfDockOverlay::enableDropPreview(bool Enable)
{
    QDF_D(QdfDockOverlay)
    d->dropPreviewEnabled = Enable;
    update();
}

bool QdfDockOverlay::dropPreviewEnabled() const
{
    QDF_D(const QdfDockOverlay)
    return d->dropPreviewEnabled;
}

void QdfDockOverlay::paintEvent(QPaintEvent *event)
{
    QDF_D(QdfDockOverlay)
    Q_UNUSED(event);
    if (!d->dropPreviewEnabled)
    {
        d->dropAreaRect = QRect();
        return;
    }

    QRect r = rect();
    const DockWidgetArea da = dropAreaUnderCursor();
    double Factor = (QdfDockOverlay::ModeContainerOverlay == d->mode) ? 3 : 2;

    switch (da)
    {
        case TopDockWidgetArea:
            r.setHeight(r.height() / Factor);
            break;
        case RightDockWidgetArea:
            r.setX(r.width() * (1 - 1 / Factor));
            break;
        case BottomDockWidgetArea:
            r.setY(r.height() * (1 - 1 / Factor));
            break;
        case LeftDockWidgetArea:
            r.setWidth(r.width() / Factor);
            break;
        case CenterDockWidgetArea:
            r = rect();
            break;
        default:
            return;
    }
    QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
    QPen Pen = painter.pen();
    Pen.setColor(Color.darker(120));
    Pen.setStyle(Qt::SolidLine);
    Pen.setWidth(1);
    Pen.setCosmetic(true);
    painter.setPen(Pen);
    Color = Color.lighter(130);
    Color.setAlpha(64);
    painter.setBrush(Color);
    painter.drawRect(r.adjusted(0, 0, -1, -1));
    d->dropAreaRect = r;
}

QRect QdfDockOverlay::dropOverlayRect() const
{
    QDF_D(const QdfDockOverlay)
    return d->dropAreaRect;
}

void QdfDockOverlay::showEvent(QShowEvent *event)
{
    QDF_D(QdfDockOverlay)
    d->cross->show();
    QFrame::showEvent(event);
}

void QdfDockOverlay::hideEvent(QHideEvent *event)
{
    QDF_D(QdfDockOverlay)
    d->cross->hide();
    QFrame::hideEvent(event);
}

bool QdfDockOverlay::event(QEvent *event)
{
    QDF_D(QdfDockOverlay)
    bool Result = QFrame::event(event);
    if (event->type() == QEvent::Polish)
    {
        d->cross->setupOverlayCross(d->mode);
    }
    return Result;
}


QDF_END_NAMESPACE