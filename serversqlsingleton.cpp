#include "serversqlsingleton.h"
ServerSqlSingleton* ServerSqlSingleton::instance = nullptr;
int ServerSqlSingleton::account_number = 0;
ServerSqlSingleton::ServerSqlSingleton(QObject* parent) : QObject(parent)
{
    if (QSqlDatabase::contains("qt_sql_default_connnection")) {
        database = QSqlDatabase::database("qt_sql_default_connnection");
    }
    else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setHostName("HOST");
        database.setUserName("USER");
        database.setPassword("114514");
    }
    if (database.open()) {
        sql_query = new QSqlQuery(database);
        QString create_accounts_table = "CREATE TABLE accounts_table (id int PRIMARY KEY, nick_name varchar(100), password varchar(20))";
        QString create_friends_table = "CREATE TABLE friends_table (id int, friend_id int, CONSTRAINT uc UNIQUE (id, friend_id))";
        if (sql_query->prepare(create_accounts_table)) {
            if (sql_query->exec()) {
                qDebug()<<"Create accounts_table succesfully";
            }
        }
        else {
            qDebug()<<"Create accounts_table command error!";
        }
        if (sql_query->prepare(create_friends_table)) {
            if (sql_query->exec()) {
                qDebug()<<"Create friends_table succesfully";
            }
        }
        else {
            qDebug()<<"Create friends_table command error!";
        }
        delete sql_query;
    }
    else {
        qDebug()<<"Database open failed!";
    }
}

QList<AccountId> ServerSqlSingleton::select_friends(AccountId target)
{
    QList<AccountId> results;
    QString select_friends = QString("SELECT friend_id FROM friends_table WHERE id = %1").arg(target);
    sql_query = new QSqlQuery(database);
    if (sql_query->prepare(select_friends)) {
        if (sql_query->exec()) {
            qDebug()<<"Select friends list succesfully";
            qDebug()<<"id ="<<target<<" friends id are ";
            while (sql_query->next())
            {
                 results.append(sql_query->value(0).toInt());
                 qDebug()<<sql_query->value(0).toInt();
            }
        }
    }
    else {
        qDebug()<<"Select friends list command error!";
    }
    delete sql_query;
    return results;
}

AccountId ServerSqlSingleton::select_id(QString target)
{
    AccountId result = -1;
    sql_query = new QSqlQuery(database);
    QString select_id = QString("SELECT id FROM accounts_table WHERE nick_name = '%1'").arg(target);
    if (sql_query->prepare(select_id)) {
        if (sql_query->exec()) {
            qDebug()<<"Select id succesfully";
            sql_query->next();
            result = sql_query->value(0).toInt();
        }
    }
    else {
        qDebug()<<"Select id command error!";
    }
    delete sql_query;
    return result;
}

AccountId ServerSqlSingleton::insert_account(QString nick_name, QString password)
{
    AccountId id = account_number++;
    QString insert_values = QString("%1, '%2', '%3'").arg(id).arg(nick_name).arg(password);
    sql_query = new QSqlQuery(database);
    QString insert_account = "INSERT INTO accounts_table VALUES (" + insert_values + ")";
    if (sql_query->prepare(insert_account)) {
        if (sql_query->exec()) {
            qDebug()<<"Insert account succesfully";
        }
    }
    else {
        qDebug()<<"Insert account command error!";
    }
    delete sql_query;
    return id;
}

bool ServerSqlSingleton::select_account(AccountId id, QString password)
{
    bool result = false;
    sql_query = new QSqlQuery(database);
    QString select_account = QString("SELECT * FROM accounts_table WHERE id = %1 AND password = '%2'").arg(id).arg(password);
    if (sql_query->prepare(select_account)) {
        if (sql_query->exec()) {
            if (sql_query->first()) {
                qDebug()<<"Select account successfully";
                result = true;
            }
        }
    }
    else {
        qDebug()<<"Select account command error!";
    }
    delete sql_query;
    return result;
}

bool ServerSqlSingleton::insert_friend(AccountId id1, AccountId id2)
{
    bool result = false;
    sql_query = new QSqlQuery(database);
    QString insert_friend = QString("INSERT INTO friends_table VALUES(%1, %2)").arg(id1).arg(id2);
    if (sql_query->prepare(insert_friend)) {
        if (sql_query->exec()) {
            qDebug()<<"Insert friend successfully";
            result = true;
        }
    }
    else {
        qDebug()<<"Insert friend command error!";
    }
    delete sql_query;
    return result;
}

bool ServerSqlSingleton::delete_friend(AccountId id1, AccountId id2)
{
    bool result = false;
    sql_query = new QSqlQuery(database);
    QString delete_friend = QString("DELETE FROM friends_table WHERE id = %1 AND friend_id = %2").arg(id1).arg(id2);
    if (sql_query->prepare(delete_friend)) {
        if (sql_query->exec()) {
            qDebug()<<"Delete friend successfully";
            result = true;
        }
    }
    else {
        qDebug()<<"Delete friend command error!";
    }
    delete sql_query;
    return result;
}
