#include "QArcDataBase.h"

QArcDataBase::QArcDataBase(QObject *parent)
    : QObject(parent)
{
    //initDataBase();
}

QArcDataBase::~QArcDataBase()
{

}

void QArcDataBase::CreateRibbonDB()
{

}

void QArcDataBase::CreateRibbonTable()
{

}

bool QArcDataBase::insertData(const Data &data, QString& strError)
{
    QSqlDatabase db;
    if (!initDataBase(db))
    {
        strError = "打开数据库失败！";
        return false;
    }

    QSqlQuery query(db);
    if (!createTable(query))
    {
        qDebug() << "Failed to create table:" << query.lastError().text();
        strError = QString("Failed to create table: %1").arg(query.lastError().text());
        return false;
    }

    QString insertSQL =
        "INSERT INTO MueuConfig (Name, AlsaName, IconPath, HotKey, Category, Pannel)"
        "VALUES (:Name, :AlsaName, :IconPath, :HotKey, :Category, :Pannel)";

    query.prepare(insertSQL);
    query.bindValue(":Name", data.m_strName);
    query.bindValue(":AlsaName", data.m_strAlsaName);
    query.bindValue(":IconPath", data.m_strIconPath);
    query.bindValue(":HotKey", data.m_strHotKey);
    query.bindValue(":Category", data.m_strCategory);
    query.bindValue(":Pannel", data.m_strPannel);

    if (!query.exec())
    {
        qDebug() << "Failed to insert value:" << query.lastError().text();
        strError = QString("Failed to insert value: %1").arg(query.lastError().text());
        return false;
    }
    return true;
}

void QArcDataBase::queryTable()
{

}

void QArcDataBase::updateData()
{

}

//void QArcDataBase::query()
//{
//
//}

bool QArcDataBase::initDataBase(QSqlDatabase &db)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");
    if (!db.open())
    {
        qDebug() << "ERROR:" << db.lastError().text();
        return false;
    }
    return true;
}

bool QArcDataBase::createTable(QSqlQuery &query)
{
    QString createTableSQL =
        "CREATE TABLE IF NOT EXISTS MueuConfig ("
        "Id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Name VARCHAR NOT NULL, "
        "AlsaName VARCHAR NOT NULL, "
        "IconPath VARCHAR NOT NULL, "
        "HotKey VARCHAR NOT NULL, "
        "Category VARCHAR NOT NULL, "
        "Pannel VARCHAR NOT NULL);";

    return query.exec(createTableSQL);
}
