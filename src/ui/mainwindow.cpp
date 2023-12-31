﻿
#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QDebug>

#include <QStatusBar>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"

#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent, bool useRibbon)
    : SARibbonMainWindow(parent)
    , bUseRibbon(useRibbon)
    //, ui(new Ui::MainWindow)
{
   // ui->setupUi(this);

    m_pArcView = new QArcGraphicsView(this);
    //ui->verticalLayout->addWidget(m_pArcView);

    m_pArcSence = new QGraphicsScene(this);//场景
    m_pArcView->setScene(m_pArcSence);

    m_pArcItem = new QArcGraphicsItem();//画布
    m_pArcSence->addItem(m_pArcItem);

    m_pArcView->setAlignment(Qt::AlignLeft | Qt::AlignTop); // 对齐QGraphicsView和QGraphicsScene坐标系


    connect(m_pArcView, &QArcGraphicsView::updateItemPosSignal, this, &MainWindow::onMousePosition);//状态栏显示坐标

    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignRight);
    //ui->statusbar->addPermanentWidget(statusLabel);

    setCentralWidget(m_pArcView);
    pStatusBar = new QStatusBar(this);
    pStatusBar->addPermanentWidget(statusLabel);
    setStatusBar(pStatusBar);

    setRibbonTheme(RibbonTheme::Office2013); // 设置主题风格
    initRightButtonGroup();
    initQuickAccessBar();

    SARibbonBar* ribbon = ribbonBar();
    //通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    //设置applicationButton
    ribbon->applicationButton()->setText(QString::fromLocal8Bit("文件"));
    ribbon->setRibbonStyle(SARibbonBar::OfficeStyle);

    ribbonBar()->setMinimumMode(false);
    

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
	setWindowIcon(QIcon(":/RibbonIcon/GA133.svg"));
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
    str = QString("当前坐标： ") + "(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
    statusLabel->setText(str);
}


void MainWindow::onRegistrationMenu()
{
    QArcRibbonRegisterDialog *dlg = new QArcRibbonRegisterDialog(this);
    dlg->setWindowTitle(QString::fromLocal8Bit("注册菜单"));
    dlg->setModal(true);// 设置为模态
    dlg->exec();        // 显示模态对话框
}

void MainWindow::initRightButtonGroup()
{
    SARibbonBar *ribbon = ribbonBar();
    SARibbonButtonGroupWidget *rightBar = ribbon->rightButtonGroup();
    //QAction *actionHelp = createAction("help", ":/icon/icon/help.svg", "help");

    QAction *actSave = new QAction(this);
    actSave->setText(QString::fromLocal8Bit("帮助"));
    actSave->setIcon(QIcon(":/RibbonIcon/help.svg"));

    //connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    rightBar->addAction(actSave);
}

QAction *MainWindow::createAction(const QString &text, const QString &icon, const QString& objName)
{
    QAction *pAction = new QAction(this);
    pAction->setText(text);
    pAction->setIcon(QIcon(icon));
    pAction->setObjectName(objName);
    return pAction;
}

void MainWindow::initQuickAccessBar()
{
    SARibbonBar *ribbon = ribbonBar();
    SARibbonQuickAccessBar *quickAccessBar = ribbon->quickAccessBar();
    quickAccessBar->addAction(createAction("save", ":/icon/icon/save.svg", "save-quickbar"));
    quickAccessBar->addSeparator();  // 分割线
    quickAccessBar->addAction(createAction("undo", ":/RibbonIcon/undo.svg", "undo"));
    quickAccessBar->addAction(createAction("redo", ":/RibbonIcon/redo.svg", "redo"));
    quickAccessBar->addSeparator();  // 分割线
}