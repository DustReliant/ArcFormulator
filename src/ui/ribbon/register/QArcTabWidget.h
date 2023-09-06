#ifndef QARCTABWIDGET_H
#define QARCTABWIDGET_H


#include <QCheckBox>
#include <QStackedWidget>
#include <QWidget>
#include <QListView>
#include <QListWidget>
#include "QArcWidgetItem.h"

class QArcTabWidget : public QWidget
{
    Q_OBJECT

public:
    enum emArcTabPosition
    {
        North,
        South,
        West,
        East
    };

    explicit QArcTabWidget(QWidget *parent = nullptr);
    ~QArcTabWidget();

public:
    //bool insertTab();
    //void removeTab();
public slots:
    void onListItemChecked(int index);

private:
    QListView* m_pListView = nullptr;
    QListWidget *m_pListWidget = nullptr;
    QCheckBox* m_pCheckBox = nullptr;
    QStackedWidget* m_pStackWidget = nullptr;

    QListWidgetItem *item = nullptr;
};
#endif // !QARCTABWIDGET_H