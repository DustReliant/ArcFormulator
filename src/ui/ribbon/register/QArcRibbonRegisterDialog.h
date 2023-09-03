#ifndef QARCRIBBONREGISTERDIALOG_H
#define QARCRIBBONREGISTERDIALOG_H

#include <QDialog>
#include "../QArcDataBase.h"
#include "QArcTabWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class QArcRibbonRegister;
}
QT_END_NAMESPACE

class QArcRibbonRegisterDialog : public QDialog
{
	Q_OBJECT

public:
    explicit QArcRibbonRegisterDialog(QWidget *parent = nullptr);
    
    ~QArcRibbonRegisterDialog();

private slots:

    void onAddIcon();

    void onRegisterButton();

    void onQuitButton();

private: 
    QArcDataBase* pDataBase = nullptr;

private:
    Ui::QArcRibbonRegister *ui;
};
#endif// !QARCRIBBONREGISTERDIALOG_H