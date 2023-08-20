#ifndef QARCGRAPHICSVIEW_H
#define QARCGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class QArcGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
    QArcGraphicsView(QWidget *parent = nullptr);
	~QArcGraphicsView();

protected:

    void mouseMoveEvent(QMouseEvent *event) override;//鼠标移动

signals:

    void updateItemPosSignal(QPointF pos); //鼠标移动信号发出
};
#endif //QARCGRAPHICSVIEW_H