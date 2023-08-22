
/************************************************************
*  
* 文件：QArcDataBase.h
* 时间：2023年8月21日
* 作者：Dust
* 
* 
*************************************************************/

#ifndef QARCDATABASE_H
#define QARCDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "pugixml.hpp"

struct Data
{
    QString m_strName;
    QString m_strAlsaName;
    QString m_strIconPath;
    QString m_strHotKey;
    QString m_strCategory;
    QString m_strPannel;
};

class QArcDataBase : public QObject
{
	Q_OBJECT

public:
	QArcDataBase(QObject *parent);
	~QArcDataBase();

	void CreateRibbonDB();

	void CreateRibbonTable();

	bool insertData(const Data &data,QString &strError);

	void queryTable();

	void updateData();

	//void query();
    void loadMenuXML();

	void saveMenuXML();

private:
	// 数据库初始化
    bool initDataBase(QSqlDatabase &db);

	bool createTable(QSqlQuery &query);

	void buildXMLFromQuery(pugi::xml_node &parent, QSqlQuery &query);

private:
    QSqlDatabase db;
    QSqlQuery query;


};
#endif //QARCDATABASE_H