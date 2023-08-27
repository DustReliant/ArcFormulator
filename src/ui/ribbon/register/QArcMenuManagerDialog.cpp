#include "QArcMenuManagerDialog.h"

QArcMenuManagerDialog::QArcMenuManagerDialog(QWidget *parent)
    : QDialog(parent)
	, ui(new Ui::QArcMenuManagerDialog)
{
	ui->setupUi(this);
}

QArcMenuManagerDialog::~QArcMenuManagerDialog()
{

}
