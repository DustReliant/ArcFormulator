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
    db.close();
    saveMenuXML();
    return true;
}

void QArcDataBase::queryTable()
{

}

void QArcDataBase::updateData()
{

}

void QArcDataBase::saveMenuXML()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");
    if (!db.open())
    {
        qDebug() << "ERROR:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("SELECT * FROM MueuConfig ORDER BY Category, Pannel");

    pugi::xml_document doc;
    // 添加XML头部
    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    pugi::xml_node root = doc.append_child("root");

    buildXMLFromQuery(root, query);

    doc.save_file("output.xml");
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

void QArcDataBase::buildXMLFromQuery(pugi::xml_node &parent, QSqlQuery &query)
{
    pugi::xml_node currentCategoryNode;
    pugi::xml_node currentPannelNode;
    QString lastCategory;
    QString lastPannel;

    while (query.next())
    {
        QString category = query.value("Category").toString();
        QString pannel = query.value("Pannel").toString();

        if (category != lastCategory)
        {
            currentCategoryNode = parent.append_child("Category");
            currentCategoryNode.append_attribute("name") = category.toStdString().c_str();
            lastCategory = category;
        }

        if (pannel != lastPannel || category != lastCategory)
        {
            currentPannelNode = currentCategoryNode.append_child("Pannel");
            currentPannelNode.append_attribute("name") = pannel.toStdString().c_str();
            lastPannel = pannel;
        }

        pugi::xml_node node = currentPannelNode.append_child("Node");
        node.append_attribute("Name") = query.value("Name").toString().toStdString().c_str();
        node.append_attribute("AlsaName") = query.value("AlsaName").toString().toStdString().c_str();
        node.append_attribute("IconPath") = query.value("IconPath").toString().toStdString().c_str();
        node.append_attribute("HotKey") = query.value("HotKey").toString().toStdString().c_str();
    }
}


void QArcDataBase::loadMenuXML()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");
    if (!db.open())
    {
        qDebug() << "ERROR:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO MueuConfig (Category, Pannel, Name, AlsaName, IconPath, HotKey) "
                  "VALUES (:Category, :Pannel, :Name, :AlsaName, :IconPath, :HotKey)");

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("output.xml");

    if (!result)
    {
        qDebug() << "XML parsed with errors";
        return;
    }

    pugi::xml_node root = doc.child("root");
    for (pugi::xml_node categoryNode: root.children("Category"))
    {
        QString category = categoryNode.attribute("name").as_string();
        for (pugi::xml_node pannelNode: categoryNode.children("Pannel"))
        {
            QString pannel = pannelNode.attribute("name").as_string();
            for (pugi::xml_node node: pannelNode.children("Node"))
            {
                QString name = node.attribute("Name").as_string();
                QString alsaName = node.attribute("AlsaName").as_string();
                QString iconPath = node.attribute("IconPath").as_string();
                QString hotKey = node.attribute("HotKey").as_string();
            }
        }
    }
}

// 待完善 后期慢慢完善
void QArcDataBase::loadMenuFromXML()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Data.db");
    if (!db.open())
    {
        qDebug() << "ERROR:" << db.lastError().text();
        return;
    }

    QSqlQuery query(db);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("output.xml");

    if (!result)
    {
        qDebug() << "XML parsed with errors";
        return;
    }

    pugi::xml_node root = doc.child("root");
    for (pugi::xml_node categoryNode: root.children("Category"))
    {
        QString category = categoryNode.attribute("name").as_string();
        for (pugi::xml_node pannelNode: categoryNode.children("Pannel"))
        {
            QString pannel = pannelNode.attribute("name").as_string();
            for (pugi::xml_node node: pannelNode.children("Node"))
            {
                QString name = node.attribute("Name").as_string();

                query.prepare("SELECT * FROM MueuConfig WHERE Name = :Name AND Category = :Category AND Pannel = :Pannel");
                query.bindValue(":Name", name);
                query.bindValue(":Category", category);
                query.bindValue(":Pannel", pannel);

                if (query.exec())
                {
                    while (query.next())
                    {
                        QString alsaName = query.value("AlsaName").toString();
                        QString iconPath = query.value("IconPath").toString();
                        QString hotKey = query.value("HotKey").toString();
                    }
                }
                else
                {
                    qDebug() << "Database query error:" << query.lastError().text();
                }
            }
        }
    }
}
