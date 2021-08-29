#ifndef SERVERSQLSINGLETON_H
#define SERVERSQLSINGLETON_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QCoreApplication>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlError>

typedef int AccountId;
class ServerSqlSingleton : public QObject
{
    Q_OBJECT
public:
    static ServerSqlSingleton* get_instance();
    // 获取好友列表
    QList<AccountId> select_friends(AccountId);
    // 根据昵称返回id
    AccountId select_id(QString);
    // 使用昵称和密码注册账号，返回id
    AccountId insert_account(QString, QString);
    // 使用id和密码登录
    bool select_account(AccountId, QString);
    // 添加好友（给id1添加好友id2
    bool insert_friend(AccountId id1, AccountId id2);
    // 删除好友（给id1删去好友id2
    bool delete_friend(AccountId id1, AccountId id2);
private:
    static ServerSqlSingleton* instance;
    static int account_number;
    QSqlDatabase database;
    QSqlQuery* sql_query;
    explicit ServerSqlSingleton(QObject* parent = nullptr);
};

inline ServerSqlSingleton *ServerSqlSingleton::get_instance(){
    if(instance == nullptr){
        instance = new ServerSqlSingleton();
    }
    return instance;
}

#endif // SERVERSQLSINGLETON_H
