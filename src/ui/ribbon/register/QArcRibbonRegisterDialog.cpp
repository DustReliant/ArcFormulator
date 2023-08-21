#include "QArcRibbonRegisterDialog.h"
#include "ui_QArcRibbonRegisterDialog.h"
#include <QFileDialog>

QArcRibbonRegisterDialog::QArcRibbonRegisterDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::QArcRibbonRegister)
{
	ui->setupUi(this);
    // 移除上方的问号
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->mButtonIcon, &QPushButton::clicked, this, &QArcRibbonRegisterDialog::onAddIcon);
}

QArcRibbonRegisterDialog::~QArcRibbonRegisterDialog()
{
    delete ui;
}

void QArcRibbonRegisterDialog::onAddIcon()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Images (*.png *.xpm *.jpg)"));
    ui->mLineEditIcon->setText(fileName);
}