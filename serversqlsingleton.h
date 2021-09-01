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
#include <QMutex>

typedef int AccountId;
class ServerSqlSingleton : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief get_instance 获取ServerSqlSingleton的单例
     * @return ServerSqlSingleton的单例对象的指针
     */
    static ServerSqlSingleton* get_instance();
    /**
     * @brief select_friends 查找qtid为target的好友
     * @param target 查找的目标qtid
     * @return qtid为target的好友列表
     */
    QList<AccountId> select_friends(AccountId target);
    /**
     * @brief select_id 查找昵称为target的qtid
     * @return 昵称为target的qtid
     */
    AccountId select_id(QString target);
    /**
     * @brief insert_account 使用昵称和密码注册账号并加入数据库
     * @return 新注册用户的qtid
     */
    AccountId insert_account(QString nickname, QString password);
    /**
     * @brief select_account 使用qtid和密码登录
     * @return 登录成功返回true,否则返回false
     */
    bool select_account(AccountId id, QString password);
    /**
     * @brief insert_friend 给qtid为id1的人加上qtid为id2的好友
     * @param id1 要加好友的人
     * @param id2 被加的好友
     * @return 加好友成功返回true,否则返回false
     */
    bool insert_friend(AccountId id1, AccountId id2);
    /**
     * @brief delete_friend 给qtid为id1的人删去qtid为id2的好友
     * @param id1 要删好友的人
     * @param id2 被删的好友
     * @return 删好友成功返回true,否则返回false
     */
    bool delete_friend(AccountId id1, AccountId id2);
    /**
     * @brief select_nickname 查找qtid为id的人的昵称
     * @param id 查找的目标qtid
     * @return 若找到则返回目标的昵称,否则返回"NO SUCH ID"
     */
    QString select_nickname(AccountId id);
private:
    static ServerSqlSingleton* instance;
    static int account_number;
    QSqlDatabase database;
    QSqlQuery* sql_query;
    /**
     * @brief ServerSqlSingleton 构造函数
     * @param parent 父亲对象的指针
     */
    explicit ServerSqlSingleton(QObject* parent = nullptr);
};

inline ServerSqlSingleton *ServerSqlSingleton::get_instance(){
    QMutex instance_mutex;
    if(instance == nullptr){
        instance_mutex.lock();
        instance = new ServerSqlSingleton();
        instance_mutex.unlock();
    }
    return instance;
}

#endif // SERVERSQLSINGLETON_H
