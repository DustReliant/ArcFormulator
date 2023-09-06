/*
* 界面主函数
* 
* 
* 
* 
* 
* 
*/


#ifndef QARCMAINWINDOW_H
#define QARCMAINWINDOW_H
#include <QWidget>
//#include "shared/ribbonwindow.h"
//#include <ribbon/qdfribbongallery.h>
//#include <ribbon/qdfribbongroup.h>
//#include <ribbon/qdfribbonpage.h>
//
//class QTextEdit;
//class QTextCharFormat;
//class QFontComboBox;
//class QComboBox;
//class QPrinter;
//
//QDF_USE_NAMESPACE

class QArcMainWindow : public QWidget
{
	Q_OBJECT

public:
    QArcMainWindow(QWidget *parent = nullptr);
	~QArcMainWindow();

private:
    //QdfRibbonPage *m_pageHome = nullptr;
    //QdfRibbonGalleryItem *p = nullptr;
};
#endif // !QARCMAINWINDOW_H