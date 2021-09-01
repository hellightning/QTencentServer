#include "serversqlsingleton.h"
ServerSqlSingleton* ServerSqlSingleton::instance = nullptr;
int ServerSqlSingleton::account_number = 0;
ServerSqlSingleton::ServerSqlSingleton(QObject* parent) : QObject(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE", "LocalDatabase");
    database.setDatabaseName(QCoreApplication::applicationDirPath() + "/LocalDatabase.db");

    if (database.open()) {
        sql_query = new QSqlQuery(database);

        if (!database.tables().contains("accounts_table")) {
            //如果没有accounts_table则新建一个
            account_number = 0;
            QString create_accounts_table = "CREATE TABLE accounts_table (id int PRIMARY KEY, nick_name varchar(100), password varchar(20))";
            if (sql_query->prepare(create_accounts_table)) {
                if (sql_query->exec()) {
                    qDebug()<<"Create accounts_table succesfully";
                }
            }
            else {
                qDebug()<<"Create accounts_table command error!";
            }
        }
        else {
            //从accounts_table中读取用户总数并更新account_number
            QString count = QString("SELECT COUNT(*) FROM accounts_table");
            sql_query->prepare(count);
            sql_query->exec();
            sql_query->next();
            account_number = sql_query->value(0).toUInt();
        }

        if (!database.tables().contains("friends_table")) {
            //如果没有friends_table则新建一个
            QString create_friends_table = "CREATE TABLE friends_table (id int, friend_id int, UNIQUE (id, friend_id))";
            qDebug()<<create_friends_table;
            if (sql_query->prepare(create_friends_table)) {
                if (sql_query->exec()) {
                    qDebug()<<"Create friends_table succesfully";
                }
            }
            else {
                qDebug()<<"Create friends_table command error!";
                qDebug()<<sql_query->lastError();
            }
        }
        delete sql_query;
    }
    else {
        qDebug()<<"Database open failed!";
    }
    qDebug()<<"account number is "<<account_number;
}

QList<AccountId> ServerSqlSingleton::select_friends(AccountId target)
{   //通过id找他的朋友并做成列表返回
    QList<AccountId> results;
//    QString select_friends = QString("SELECT id FROM accounts_table"); //test
    QString select_friends = QString("SELECT friend_id FROM friends_table WHERE id = %1").arg(target);
    sql_query = new QSqlQuery(database);
    if (sql_query->prepare(select_friends)) {
        if (sql_query->exec()) {
            while (sql_query->next())
            {
                 results.append(sql_query->value(0).toInt());
            }
            qDebug()<<"Select friends list succesfully";
        }
    }
    else {
        qDebug()<<"Select friends list command error!";
    }
    delete sql_query;
    return results;
}

AccountId ServerSqlSingleton::select_id(QString target)
{   //通过昵称找到他的id
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
{   //通过昵称和密码新建账户并返回id
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
{   //通过id和密码查询数据库是否正确，并返回查询结果
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
{   //给id为id1的人加上id为id2的人的好友
    bool result = false;
    sql_query = new QSqlQuery(database);
    QString search = QString("SELECT * FROM accounts_table WHERE id = %1").arg(id2);
    if (sql_query->prepare(search)) {
        if (sql_query->exec()) {
            if (!sql_query->next()) {
                qDebug()<<"No such person whose id is "<<id2;
                return false;
            }
        }
    }
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
{   //给id为id1的人删去id为id2的人的好友
    bool result = false;
    sql_query = new QSqlQuery(database);
    QString search = QString("SELECT * FROM accounts_table WHERE id = %1").arg(id2);
    if (sql_query->prepare(search)) {
        if (sql_query->exec()) {
            if (!sql_query->next()) {
                qDebug()<<"No such person whose id is "<<id2;
                return false;
            }
        }
    }
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

QString ServerSqlSingleton::select_nickname(AccountId id)
{
    QString result = QString("NO SUCH ID");
    sql_query = new QSqlQuery(database);
    QString select_nickname = QString("SELECT nick_name FROM accounts_table WHERE id = %1").arg(id);
    qDebug()<< select_nickname;
    if (sql_query->prepare(select_nickname)) {
        if (sql_query->exec()) {
            sql_query->next();
            qDebug()<<"Result is " << sql_query->value(0).toString();
            result = sql_query->value(0).toString();
            qDebug()<<"Select nickname successfully";
        }
    }
    else {
        qDebug()<<"Select nickname command error!";
    }
    return result;
}
