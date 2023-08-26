
/*
* 
* ≤Àµ•π‹¿Ì∆˜UI
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
*/




#ifndef QARCMENUMANAGERDIALOG_H
#define QARCMENUMANAGERDIALOG_H

#include <QDialog>
#include "ui_QArcMenuManagerDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class QArcMenuManagerDialog;
}
QT_END_NAMESPACE

class QArcMenuManagerDialog : public QDialog
{
	Q_OBJECT

public:
	QArcMenuManagerDialog(QWidget *parent = nullptr);
	~QArcMenuManagerDialog();





private:
	Ui::QArcMenuManagerDialog* ui;
};
#endif//QARCMENUMANAGERDIALOG_H