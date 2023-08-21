#include "QArcRibbonRegisterDialog.h"
#include "ui_QArcRibbonRegisterDialog.h"

QArcRibbonRegisterDialog::QArcRibbonRegisterDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::QArcRibbonRegister)
{
	ui->setupUi(this);
    // 移除上方的问号
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

QArcRibbonRegisterDialog::~QArcRibbonRegisterDialog()
{
    delete ui;
}
