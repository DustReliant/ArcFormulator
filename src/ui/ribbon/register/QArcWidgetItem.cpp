#include "QArcWidgetItem.h"
#include <QMouseEvent>

QArcWidgetItem::QArcWidgetItem(int index, QString text, QWidget *parent)
    : QWidget(parent)
    ,index(index)
{
    pCheckBox = new QCheckBox(this);
    pCheckBox->setText(text);
}

QArcWidgetItem::~QArcWidgetItem()
{
    if (pCheckBox)
    {
        delete pCheckBox;
        pCheckBox = nullptr;
    }
}

int QArcWidgetItem::getIndex()
{
    return index;
}

QCheckBox* QArcWidgetItem::checkBox()
{
    return pCheckBox;
}
QString QArcWidgetItem::checkBoxText()
{
    return pCheckBox->text();
}
void QArcWidgetItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit listItemClicked(index);
    }
}
