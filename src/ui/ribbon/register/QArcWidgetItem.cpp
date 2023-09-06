#include "QArcWidgetItem.h"
#include <QMouseEvent>

QArcWidgetItem::QArcWidgetItem(int index, QString text, QWidget *parent)
    : QWidget(parent)
    ,index(index)
{
    pCheckBox = new QCheckBox(this);
    m_bHideFlag = true;
    if (m_bHideFlag)
    {
        pCheckBox->setStyleSheet("#checkBox::indicator{width: 0px;}");//影藏QCheckBox的复选框
    }
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
    // 这里还存在bug 未隐藏  待解决 
    if (hide)
    {
        pCheckBox->setStyleSheet("#checkBox::indicator{width: 0px;}");//影藏QCheckBox的复选框
    }
}
void QArcWidgetItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit listItemClicked(index);
    }
}
