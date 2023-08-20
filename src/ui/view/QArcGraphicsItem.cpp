#include "QArcGraphicsItem.h"

QArcGraphicsItem::QArcGraphicsItem()
{

}

QArcGraphicsItem::~QArcGraphicsItem()
{

}

void QArcGraphicsItem::setRect(const QRectF &rect)
{
    m_Rect = rect;
}

void QArcGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
    painter->setRenderHint(QPainter::Antialiasing);
    QPen show_pen;
    show_pen.setColor(Qt::red);
    show_pen.setStyle(Qt::SolidLine);
    painter->setPen(show_pen);
    // 设置起点、控制点和终点
    QPointF start(20, 150);
    QPointF ctrl(50, 50);
    QPointF end(100, 120);

    // 创建QPainterPath对象并移动到起点
    QPainterPath path(start);

    // 添加二次贝塞尔曲线
    path.quadTo(ctrl, end);

    // 绘制路径
    painter->drawPath(path);

    QPointF point(10, 10);
    painter->drawText(point, "Hello world!");
}

QRectF QArcGraphicsItem::boundingRect() const
{
    return m_Rect;
}

