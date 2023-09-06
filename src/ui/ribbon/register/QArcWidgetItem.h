#ifndef QARCWIDGETITEM_H
#define QARCWIDGETITEM_H
#include <QWidget>
#include <QCheckBox>

class QArcWidgetItem : public QWidget
{
	Q_OBJECT

public:
    QArcWidgetItem(int index, QString text, QWidget *parent = nullptr);
	~QArcWidgetItem();

public:
    int getIndex();
    QCheckBox* checkBox();
    QString checkBoxText();


signals:
    void listItemClicked(int);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    int index;
    QCheckBox *pCheckBox = nullptr;
};
#endif//QARCWIDGETITEM_H