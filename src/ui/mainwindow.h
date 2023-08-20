
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "view/qarcgraphicsview.h"
#include "view/qarcgraphicsitem.h"
#include <QGraphicsScene>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onMousePosition(QPointF pos);

private:
    QArcGraphicsView* m_pArcView = nullptr;
    QGraphicsScene* m_pArcSence = nullptr;   //³¡¾°
    QArcGraphicsItem* m_pArcItem = nullptr;  //»­²¼
    QLabel* statusLabel = nullptr;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
