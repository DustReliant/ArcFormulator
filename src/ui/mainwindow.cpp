
#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QDebug>

#include <QStatusBar>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"

#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : SARibbonMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
   // ui->setupUi(this);

    m_pArcView = new QArcGraphicsView(this);
    //ui->verticalLayout->addWidget(m_pArcView);

    m_pArcSence = new QGraphicsScene(this);//场景
    m_pArcView->setScene(m_pArcSence);

    m_pArcItem = new QArcGraphicsItem();//画布
    m_pArcSence->addItem(m_pArcItem);


    //connect(m_pArcView, &QArcGraphicsView::updateItemPosSignal, this, &MainWindow::onMousePosition);//状态栏显示坐标

    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignRight);
    //ui->statusbar->addPermanentWidget(statusLabel);

    setCentralWidget(m_pArcView);
    setStatusBar(new QStatusBar());
    SARibbonBar* ribbon = ribbonBar();
    //通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    //设置applicationButton
    ribbon->applicationButton()->setText(QString::fromLocal8Bit("文件"));

    //添加主标签页 - 通过addCategoryPage工厂函数添加
    SARibbonCategory* categoryMain = ribbon->addCategoryPage(QString::fromLocal8Bit("主页"));
    categoryMain->setObjectName(("categoryMain"));
    //createCategoryMain(categoryMain);
    //使用addPannel函数来创建SARibbonPannel，效果和new SARibbonPannel再addPannel一样
    SARibbonPannel *pannel1 = categoryMain->addPannel(QString::fromLocal8Bit("菜单"));
    QAction *actSave = new QAction(this);
    actSave->setText(QString::fromLocal8Bit("注册"));
    actSave->setIcon(QIcon(":/icon/icon/save.svg"));
    actSave->setObjectName("actSave");
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    connect(actSave, &QAction::triggered, this, &MainWindow::onRegistrationMenu);
    pannel1->addLargeAction(actSave);


    //添加其他标签页 - 直接new SARibbonCategory添加
    SARibbonCategory* categoryOther = new SARibbonCategory();
    categoryOther->setCategoryName(QString::fromLocal8Bit("其他"));
    categoryOther->setObjectName(("categoryOther"));
    //createCategoryOther(categoryOther);
    //ribbon->addCategoryPage(categoryOther);

    //添加删除标签页
    SARibbonCategory* categoryDelete = new SARibbonCategory();

    categoryDelete->setCategoryName(QString::fromLocal8Bit("删除"));
    categoryDelete->setObjectName(("categoryDelete"));
    //ribbon->addCategoryPage(categoryDelete);
    //添加尺寸标签页
    SARibbonCategory* categorySize = new SARibbonCategory();

    categorySize->setCategoryName(QString::fromLocal8Bit("size"));
    categorySize->setObjectName(("categorySize"));
    //ribbon->addCategoryPage(categorySize);

    setMinimumWidth(900);
    setMinimumHeight(600);
    //showMaximized();
}

MainWindow::~MainWindow()
{
    if (m_pArcItem)
    {
        delete m_pArcItem;
        m_pArcItem = nullptr;
    }
    if (m_customizeWidget)
    {
        delete m_customizeWidget;
        m_customizeWidget = nullptr;
    }
    //delete ui;
}


void MainWindow::onMousePosition(QPointF pos)
{
    QString str;
    if (pos.isNull())
    {
        str = QString::fromLocal8Bit("当前坐标：(0,0)");
    }
    str = QString::fromLocal8Bit("当前坐标： ") + "(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
    statusLabel->setText(str);
}


void MainWindow::onRegistrationMenu()
{
    QArcRibbonRegisterDialog *dlg = new QArcRibbonRegisterDialog(this);
    dlg->setWindowTitle(QString::fromLocal8Bit("注册菜单"));
    dlg->setModal(true);// 设置为模态
    dlg->exec();        // 显示模态对话框
}