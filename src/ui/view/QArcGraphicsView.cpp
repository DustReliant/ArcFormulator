#include "QArcGraphicsView.h"

QArcGraphicsView::QArcGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
    //viewport()->update();
}

QArcGraphicsView::~QArcGraphicsView()
{

}

void QArcGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit updateItemPosSignal(event->pos());
}
