
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "view/qarcgraphicsview.h"
#include "view/qarcgraphicsitem.h"
#include <QGraphicsScene>
#include <QLabel>

#include "ribbon/register/QArcRibbonRegisterdialog.h"

#include "SARibbonMainWindow.h"
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class SARibbonActionsManager;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class QTextEdit;


//QT_BEGIN_NAMESPACE
//namespace Ui { class MainWindow; }
//QT_END_NAMESPACE

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void onMousePosition(QPointF pos);


private slots:
    //粕等蛁聊偌聽
    void onRegistrationMenu();

private:
    QArcGraphicsView* m_pArcView = nullptr;
    QGraphicsScene* m_pArcSence = nullptr;   //部劓
    QArcGraphicsItem* m_pArcItem = nullptr;  //賒票
    QLabel* statusLabel = nullptr;

private:
    //Ui::MainWindow *ui;
    SARibbonCustomizeWidget* m_customizeWidget = nullptr;
};

#endif // MAINWINDOW_H
