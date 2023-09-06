#include "QArcWidgetItem.h"
#include <QMouseEvent>

QArcWidgetItem::QArcWidgetItem(int index, QString text, QWidget *parent)
    : QWidget(parent)
    ,index(index)
{
    pCheckBox = new QCheckBox(this);
    m_bHideFlag = true;
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

void QArcWidgetItem::setHideBox(bool hide)
{
    m_bHideFlag = hide;
    if (hide)
    {
        pCheckBox->setStyleSheet("QCheckBox::indicator { width: 0px; }");//Ó°²ØQCheckBoxµÄ¸´Ñ¡¿ò
    }
}
void QArcWidgetItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit listItemClicked(index);
    }
}
