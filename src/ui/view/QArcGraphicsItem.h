#ifndef QARCGRAPHICSITEM_H
#define QARCGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QtGlobal>


class QArcGraphicsItem : public QGraphicsItem 
{
public:
    QArcGraphicsItem();
	
	~QArcGraphicsItem();

	void setRect(const QRectF &rect);



protected:

	// @brief 绘制图形项的内容。
	//
	// 这个函数被 QGraphicsView 框架调用，用于在视图中渲染图形项。
	// 
	// @param painter 指向进行绘制操作的 QPainter 对象的指针。
	// @param option 提供绘制选项（如图形项的状态）的 QStyleOptionGraphicsItem 对象的指针。
	// @param widget 如果这个图形项在一个部件中被绘制，则此参数指向该部件。默认为 nullptr。
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // @brief boundingRect 用于返回图形项的边界矩形。
    //
    // 这个边界矩形定义了图形项的外界范围，是图形项在场景（scene）中所占用的矩形区域。
    // Qt的图形视图框架使用这个矩形进行各种运算，比如碰撞检测、渲染优化等。
    // 边界矩形应该尽可能地紧凑，不应包含图形项不会绘制或交互的区域。
    // 
    // @return 返回一个QRectF对象，代表了图形项的边界矩形。
    // 边界矩形的坐标是相对于图形项自身的坐标系统（而非场景或视图的坐标系统）。
	virtual QRectF boundingRect() const override;


private:
	double calculateAngle(const QPointF& center, const QPointF& point);


private:
    QRectF m_Rect;
};
#endif//QARCGRAPHICSITEM_H