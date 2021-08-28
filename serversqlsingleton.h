#ifndef SERVERSQLSINGLETON_H
#define SERVERSQLSINGLETON_H

#include <QObject>
#include <QSqlDatabase>

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
    // 添加好友（待定
    bool insert_friend(AccountId);
    // 删除好友（待定
    bool delete_friend(AccountId);
private:
    static ServerSqlSingleton* instance;
    static int account_number;
    QSqlDatabase* accounts_table;
    QSqlDatabase* friends_table;
    explicit ServerSqlSingleton();
};

inline ServerSqlSingleton *ServerSqlSingleton::get_instance(){
    if(instance == nullptr){
        instance = new ServerSqlSingleton();
    }
    return instance;
}

#endif // SERVERSQLSINGLETON_H
