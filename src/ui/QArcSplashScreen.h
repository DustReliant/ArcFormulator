#pragma once

#include <QObject>

class QArcSplashScreen : public QObject
{
	Q_OBJECT

public:
	QArcSplashScreen(QObject *parent);
	~QArcSplashScreen();
};
