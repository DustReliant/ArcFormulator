#include "qdfdockoverlaycross.h"
#include <QCursor>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMap>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWidget>
#include <QWindow>
#include <QtGlobal>

QDF_BEGIN_NAMESPACE

class QdfDockOverlayCrossPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockOverlayCross)

    QdfDockOverlayCrossPrivate() = default;
    QPoint areaGridPosition(const DockWidgetArea area);
    QColor defaultIconColor(QdfDockOverlayCross::IconColor colorIndex);
    QColor iconColor(QdfDockOverlayCross::IconColor colorIndex);
    qreal dropIndicatiorWidth(QLabel *label) const;
    QWidget *createDropIndicatorWidget(DockWidgetArea DockWidgetArea, QdfDockOverlay::Mode mode);
    void updateDropIndicatorIcon(QWidget *DropIndicatorWidget);
    QPixmap createHighDpiDropIndicatorPixmap(const QSizeF &size, DockWidgetArea DockWidgetArea, QdfDockOverlay::Mode mode);

private:
    QdfDockOverlay::Mode mode = QdfDockOverlay::ModeDockAreaOverlay;
    QdfDockOverlay *dockOverlay;
    QHash<DockWidgetArea, QWidget *> dropIndicatorWidgets;
    QGridLayout *gridLayout;
    QColor iconColors[5];
    bool updateRequired = false;
    double lastDevicePixelRatio = 0.1;

private:
    Q_DISABLE_COPY(QdfDockOverlayCrossPrivate)
};

QPoint QdfDockOverlayCrossPrivate::areaGridPosition(const DockWidgetArea area)
{
    if (QdfDockOverlay::ModeDockAreaOverlay == mode)
    {
        switch (area)
        {
            case TopDockWidgetArea:
                return QPoint(1, 2);
            case RightDockWidgetArea:
                return QPoint(2, 3);
            case BottomDockWidgetArea:
                return QPoint(3, 2);
            case LeftDockWidgetArea:
                return QPoint(2, 1);
            case CenterDockWidgetArea:
                return QPoint(2, 2);
            default:
                return QPoint();
        }
    }
    else
    {
        switch (area)
        {
            case TopDockWidgetArea:
                return QPoint(0, 2);
            case RightDockWidgetArea:
                return QPoint(2, 4);
            case BottomDockWidgetArea:
                return QPoint(4, 2);
            case LeftDockWidgetArea:
                return QPoint(2, 0);
            case CenterDockWidgetArea:
                return QPoint(2, 2);
            default:
                return QPoint();
        }
    }
}

QColor QdfDockOverlayCrossPrivate::defaultIconColor(QdfDockOverlayCross::IconColor colorIndex)
{
    QDF_Q(QdfDockOverlayCross)
    QPalette pal = q->palette();
    switch (colorIndex)
    {
        case QdfDockOverlayCross::FrameColor:
            return pal.color(QPalette::Active, QPalette::Highlight);
        case QdfDockOverlayCross::WindowBackgroundColor:
            return pal.color(QPalette::Active, QPalette::Base);
        case QdfDockOverlayCross::OverlayColor:
            {
                QColor color = pal.color(QPalette::Active, QPalette::Highlight);
                color.setAlpha(64);
                return color;
            }
            break;

        case QdfDockOverlayCross::ArrowColor:
            return pal.color(QPalette::Active, QPalette::Base);
        case QdfDockOverlayCross::ShadowColor:
            return QColor(0, 0, 0, 64);
        default:
            return QColor();
    };
}

QColor QdfDockOverlayCrossPrivate::iconColor(QdfDockOverlayCross::IconColor colorIndex)
{
    QColor color = iconColors[colorIndex];
    if (!color.isValid())
    {
        color = defaultIconColor(colorIndex);
        iconColors[colorIndex] = color;
    }
    return color;
}

qreal QdfDockOverlayCrossPrivate::dropIndicatiorWidth(QLabel *l) const
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    Q_UNUSED(l)
    return 40;
#else
    return static_cast<qreal>(l->fontMetrics().height()) * 3.f;
#endif
}

QWidget *QdfDockOverlayCrossPrivate::createDropIndicatorWidget(DockWidgetArea area,
                                                               QdfDockOverlay::Mode mode)
{
    QLabel *l = new QLabel();
    l->setObjectName("DockWidgetAreaLabel");

    const qreal metric = dropIndicatiorWidth(l);
    const QSizeF size(metric, metric);

    l->setPixmap(createHighDpiDropIndicatorPixmap(size, area, mode));
    l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    l->setAttribute(Qt::WA_TranslucentBackground);
    l->setProperty("dockWidgetArea", area);
    return l;
}
void QdfDockOverlayCrossPrivate::updateDropIndicatorIcon(QWidget *DropIndicatorWidget)
{
    QLabel *l = qobject_cast<QLabel *>(DropIndicatorWidget);
    const qreal metric = dropIndicatiorWidth(l);
    const QSizeF size(metric, metric);

    int area = l->property("dockWidgetArea").toInt();
    l->setPixmap(createHighDpiDropIndicatorPixmap(size, (DockWidgetArea) area, mode));
}

QPixmap QdfDockOverlayCrossPrivate::createHighDpiDropIndicatorPixmap(const QSizeF &size, DockWidgetArea area,
                                                                     QdfDockOverlay::Mode mode)
{
    QDF_Q(QdfDockOverlayCross)
    QColor borderColor = iconColor(QdfDockOverlayCross::FrameColor);
    QColor backgroundColor = iconColor(QdfDockOverlayCross::WindowBackgroundColor);

#if QT_VERSION >= 0x050600
    double DevicePixelRatio = q->window()->devicePixelRatioF();
#else
    double DevicePixelRatio = q->window()->devicePixelRatio();
#endif
    QSizeF PixmapSize = size * DevicePixelRatio;
    QPixmap pm(PixmapSize.toSize());
    pm.fill(QColor(0, 0, 0, 0));

    QPainter painter(&pm);
    QPen pen = painter.pen();
    QRectF ShadowRect(pm.rect());
    QRectF baseRect;
    baseRect.setSize(ShadowRect.size() * 0.7);
    baseRect.moveCenter(ShadowRect.center());

    // Fill
    QColor ShadowColor = iconColor(QdfDockOverlayCross::ShadowColor);
    if (ShadowColor.alpha() == 255)
    {
        ShadowColor.setAlpha(64);
    }
    painter.fillRect(ShadowRect, ShadowColor);

    // Drop area rect.
    painter.save();
    QRectF areaRect;
    QLineF areaLine;
    QRectF nonAreaRect;
    switch (area)
    {
        case TopDockWidgetArea:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
            nonAreaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
            break;
        case RightDockWidgetArea:
            areaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
            nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
            break;
        case BottomDockWidgetArea:
            areaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
            nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
            areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
            break;
        case LeftDockWidgetArea:
            areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
            nonAreaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
            areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
            break;
        default:
            break;
    }

    QSizeF baseSize = baseRect.size();
    if (QdfDockOverlay::ModeContainerOverlay == mode && area != CenterDockWidgetArea)
    {
        baseRect = areaRect;
    }

    painter.fillRect(baseRect, backgroundColor);
    if (areaRect.isValid())
    {
        pen = painter.pen();
        pen.setColor(borderColor);
        QColor color = iconColor(QdfDockOverlayCross::OverlayColor);
        if (color.alpha() == 255)
        {
            color.setAlpha(64);
        }
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(areaRect);

        pen = painter.pen();
        pen.setWidth(1);
        pen.setColor(borderColor);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawLine(areaLine);
    }
    painter.restore();

    painter.save();
    // Draw outer border
    pen = painter.pen();
    pen.setColor(borderColor);
    pen.setWidth(1);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawRect(baseRect);

    // draw window title bar
    painter.setBrush(borderColor);
    QRectF FrameRect(baseRect.topLeft(), QSizeF(baseRect.width(), baseSize.height() / 10));
    painter.drawRect(FrameRect);
    painter.restore();

    // Draw arrow for outer container drop indicators
    if (QdfDockOverlay::ModeContainerOverlay == mode && area != CenterDockWidgetArea)
    {
        QRectF ArrowRect;
        ArrowRect.setSize(baseSize);
        ArrowRect.setWidth(ArrowRect.width() / 4.6);
        ArrowRect.setHeight(ArrowRect.height() / 2);
        ArrowRect.moveCenter(QPointF(0, 0));
        QPolygonF Arrow;
        Arrow << ArrowRect.topLeft()
              << QPointF(ArrowRect.right(), ArrowRect.center().y())
              << ArrowRect.bottomLeft();
        painter.setPen(Qt::NoPen);
        painter.setBrush(iconColor(QdfDockOverlayCross::ArrowColor));
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.translate(nonAreaRect.center().x(), nonAreaRect.center().y());

        switch (area)
        {
            case TopDockWidgetArea:
                painter.rotate(-90);
                break;
            case RightDockWidgetArea:
                break;
            case BottomDockWidgetArea:
                painter.rotate(90);
                break;
            case LeftDockWidgetArea:
                painter.rotate(180);
                break;
            default:
                break;
        }

        painter.drawPolygon(Arrow);
    }

    pm.setDevicePixelRatio(DevicePixelRatio);
    return pm;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
int QdfDockOverlayCross::areaAlignment(const DockWidgetArea &area)
{
    switch (area)
    {
        case TopDockWidgetArea:
            return (int) Qt::AlignHCenter | Qt::AlignBottom;
        case RightDockWidgetArea:
            return (int) Qt::AlignLeft | Qt::AlignVCenter;
        case BottomDockWidgetArea:
            return (int) Qt::AlignHCenter | Qt::AlignTop;
        case LeftDockWidgetArea:
            return (int) Qt::AlignRight | Qt::AlignVCenter;
        case CenterDockWidgetArea:
            return (int) Qt::AlignCenter;
        default:
            return Qt::AlignCenter;
    }
}

QdfDockOverlayCross::QdfDockOverlayCross(QdfDockOverlay *overlay)
    : QWidget(overlay->parentWidget())
{
    QDF_INIT_PRIVATE(QdfDockOverlayCross)
    QDF_D(QdfDockOverlayCross)

    d->dockOverlay = overlay;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
    setWindowTitle("DockOverlayCross");
    setAttribute(Qt::WA_TranslucentBackground);

    d->gridLayout = new QGridLayout();
    d->gridLayout->setSpacing(0);
    setLayout(d->gridLayout);
}

QdfDockOverlayCross::~QdfDockOverlayCross()
{
    QDF_FINI_PRIVATE()
}

void QdfDockOverlayCross::setupOverlayCross(QdfDockOverlay::Mode mode)
{
    QDF_D(QdfDockOverlayCross)
    d->mode = mode;

    QHash<DockWidgetArea, QWidget *> areaWidgets;
    areaWidgets.insert(TopDockWidgetArea, d->createDropIndicatorWidget(TopDockWidgetArea, mode));
    areaWidgets.insert(RightDockWidgetArea, d->createDropIndicatorWidget(RightDockWidgetArea, mode));
    areaWidgets.insert(BottomDockWidgetArea, d->createDropIndicatorWidget(BottomDockWidgetArea, mode));
    areaWidgets.insert(LeftDockWidgetArea, d->createDropIndicatorWidget(LeftDockWidgetArea, mode));
    areaWidgets.insert(CenterDockWidgetArea, d->createDropIndicatorWidget(CenterDockWidgetArea, mode));
#if QT_VERSION >= 0x050600
    d->lastDevicePixelRatio = devicePixelRatioF();
#else
    d->lastDevicePixelRatio = devicePixelRatio();
#endif
    setAreaWidgets(areaWidgets);
    d->updateRequired = false;
}

void QdfDockOverlayCross::updateOverlayIcons()
{
    QDF_D(QdfDockOverlayCross)
    if (windowHandle()->devicePixelRatio() == d->lastDevicePixelRatio)
    {
        return;
    }

    for (auto &widget: d->dropIndicatorWidgets)
    {
        d->updateDropIndicatorIcon(widget);
    }
#if QT_VERSION >= 0x050600
    d->lastDevicePixelRatio = devicePixelRatioF();
#else
    d->lastDevicePixelRatio = devicePixelRatio();
#endif
}

void QdfDockOverlayCross::setIconColor(IconColor colorIndex, const QColor &color)
{
    QDF_D(QdfDockOverlayCross)
    d->iconColors[colorIndex] = color;
    d->updateRequired = true;
}

QColor QdfDockOverlayCross::iconColor(IconColor colorIndex) const
{
    QDF_D(const QdfDockOverlayCross)
    return d->iconColors[colorIndex];
}

void QdfDockOverlayCross::setAreaWidgets(const QHash<DockWidgetArea, QWidget *> &widgets)
{
    QDF_D(QdfDockOverlayCross)
    QMutableHashIterator<DockWidgetArea, QWidget *> i(d->dropIndicatorWidgets);
    while (i.hasNext())
    {
        i.next();
        QWidget *widget = i.value();
        d->gridLayout->removeWidget(widget);
        delete widget;
        i.remove();
    }

    d->dropIndicatorWidgets = widgets;
    QHashIterator<DockWidgetArea, QWidget *> i2(d->dropIndicatorWidgets);
    while (i2.hasNext())
    {
        i2.next();
        const DockWidgetArea area = i2.key();
        QWidget *widget = i2.value();
        QPoint p = d->areaGridPosition(area);
        d->gridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment) areaAlignment(area));
    }

    if (QdfDockOverlay::ModeDockAreaOverlay == d->mode)
    {
        d->gridLayout->setContentsMargins(0, 0, 0, 0);
        d->gridLayout->setRowStretch(0, 1);
        d->gridLayout->setRowStretch(1, 0);
        d->gridLayout->setRowStretch(2, 0);
        d->gridLayout->setRowStretch(3, 0);
        d->gridLayout->setRowStretch(4, 1);

        d->gridLayout->setColumnStretch(0, 1);
        d->gridLayout->setColumnStretch(1, 0);
        d->gridLayout->setColumnStretch(2, 0);
        d->gridLayout->setColumnStretch(3, 0);
        d->gridLayout->setColumnStretch(4, 1);
    }
    else
    {
        d->gridLayout->setContentsMargins(4, 4, 4, 4);
        d->gridLayout->setRowStretch(0, 0);
        d->gridLayout->setRowStretch(1, 1);
        d->gridLayout->setRowStretch(2, 1);
        d->gridLayout->setRowStretch(3, 1);
        d->gridLayout->setRowStretch(4, 0);

        d->gridLayout->setColumnStretch(0, 0);
        d->gridLayout->setColumnStretch(1, 1);
        d->gridLayout->setColumnStretch(2, 1);
        d->gridLayout->setColumnStretch(3, 1);
        d->gridLayout->setColumnStretch(4, 0);
    }
    reset();
}

DockWidgetArea QdfDockOverlayCross::cursorLocation() const
{
    QDF_D(const QdfDockOverlayCross)
    const QPoint pos = mapFromGlobal(QCursor::pos());
    QHashIterator<DockWidgetArea, QWidget *> iter(d->dropIndicatorWidgets);
    while (iter.hasNext())
    {
        iter.next();
        if (d->dockOverlay->allowedAreas().testFlag(iter.key()) && iter.value() && iter.value()->isVisible() && iter.value()->geometry().contains(pos))
        {
            return iter.key();
        }
    }
    return InvalidDockWidgetArea;
}

void QdfDockOverlayCross::showEvent(QShowEvent *)
{
    QDF_D(QdfDockOverlayCross)
    if (d->updateRequired)
    {
        setupOverlayCross(d->mode);
    }
    this->updatePosition();
}

void QdfDockOverlayCross::updatePosition()
{
    QDF_D(QdfDockOverlayCross)
    resize(d->dockOverlay->size());
    QPoint TopLeft = d->dockOverlay->pos();
    QPoint Offest((this->width() - d->dockOverlay->width()) / 2,
                  (this->height() - d->dockOverlay->height()) / 2);
    QPoint CrossTopLeft = TopLeft - Offest;
    move(CrossTopLeft);
}

void QdfDockOverlayCross::reset()
{
    QDF_D(QdfDockOverlayCross)
    QList<DockWidgetArea> allAreas;
    allAreas << TopDockWidgetArea << RightDockWidgetArea
             << BottomDockWidgetArea << LeftDockWidgetArea << CenterDockWidgetArea;
    const DockWidgetAreas allowedAreas = d->dockOverlay->allowedAreas();

    for (int i = 0; i < allAreas.count(); ++i)
    {
        QPoint p = d->areaGridPosition(allAreas.at(i));
        QLayoutItem *item = d->gridLayout->itemAtPosition(p.x(), p.y());
        QWidget *w = nullptr;
        if (item && (w = item->widget()) != nullptr)
        {
            w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
        }
    }
}

void QdfDockOverlayCross::setIconColors(const QString &Colors)
{
    QDF_D(QdfDockOverlayCross)
    static const QMap<QString, int> ColorCompenentStringMap{
            {     "Frame",            QdfDockOverlayCross::FrameColor},
            {"Background", QdfDockOverlayCross::WindowBackgroundColor},
            {   "Overlay",          QdfDockOverlayCross::OverlayColor},
            {     "Arrow",            QdfDockOverlayCross::ArrowColor},
            {    "Shadow",           QdfDockOverlayCross::ShadowColor}
    };

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    auto SkipEmptyParts = QString::SkipEmptyParts;
#else
    auto SkipEmptyParts = Qt::SkipEmptyParts;
#endif
    auto ColorList = Colors.split(' ', SkipEmptyParts);
    for (const auto &ColorListEntry: ColorList)
    {
        auto ComponentColor = ColorListEntry.split('=', SkipEmptyParts);
        int Component = ColorCompenentStringMap.value(ComponentColor[0], -1);
        if (Component < 0)
        {
            continue;
        }
        d->iconColors[Component] = QColor(ComponentColor[1]);
    }

    d->updateRequired = true;
}

QString QdfDockOverlayCross::iconColors() const
{
    return QString();
}

QDF_END_NAMESPACE