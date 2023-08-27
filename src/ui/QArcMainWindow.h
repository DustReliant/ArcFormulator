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
#include <QObject>

class QArcMainWindow : public QObject
{
	Q_OBJECT

public:
	QArcMainWindow(QObject *parent);
	~QArcMainWindow();
};
#endif // !QARCMAINWINDOW_H