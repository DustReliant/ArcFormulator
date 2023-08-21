




#ifndef QARCDATABASE_H
#define QARCDATABASE_H

#include <QObject>

class QArcDataBase : public QObject
{
	Q_OBJECT

public:
	QArcDataBase(QObject *parent);
	~QArcDataBase();

	void CreateRibbonDB();

	void CreateRibbonTable();

	void insertData();

	void queryTable();

	void updateData();

	void query();

};
#endif //QARCDATABASE_H