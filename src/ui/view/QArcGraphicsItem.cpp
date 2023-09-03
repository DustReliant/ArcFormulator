#include "QArcGraphicsItem.h"
#include <qmath.h>

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

// 计算两点之间的弧度角
double QArcGraphicsItem::calculateAngle(const QPointF& center, const QPointF& point)
{
	double dx = point.x() - center.x();
	double dy = point.y() - center.y();
	return atan2(dy, dx);
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

    QPointF point(100, 100);
    painter->drawText(point, "Hello world!");


    // 假设有三个点的坐标，你需要将它们替换为你的实际坐标
	QPointF		   pointA(100, 100);
	QPointF		   pointB(200, 200);
	QPointF		   pointC(300, 100);

	// 计算三角形的中心点，这里假设三个点是顶点
	QPointF		   center((pointA.x() + pointB.x() + pointC.x()) / 3,
						  (pointA.y() + pointB.y() + pointC.y()) / 3);

	// 计算两点之间的角度差
	double	angleA = calculateAngle(center, pointA);
	double	angleB = calculateAngle(center, pointB);

	// 计算弧度差，确保结果在[0, 2*π]范围内
	double angleDifference = fmod((angleB - angleA + 2 * M_PI), (2 * M_PI));

	// 创建一个圆弧的绘制路径
	QPainterPath   paths;
	paths.arcTo(50, 50, 30, 30, qRadiansToDegrees(angleA), qRadiansToDegrees(angleDifference));
	painter->drawPath(paths);
}

QRectF QArcGraphicsItem::boundingRect() const
{
    return m_Rect;
}

