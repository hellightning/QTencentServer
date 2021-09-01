/**
 * @brief 聊天系统的Server单例类
 */
#ifndef TCPSERVERSINGLETON_H
#define TCPSERVERSINGLETON_H

#include <QTcpServer>
#include <QHostAddress>
#include <QHostInfo>
#include <QTimerEvent>
#include "servertcpsocket.h"
#include "serversqlsingleton.h"

typedef int QtId;
typedef QString Message;
class TcpServerSingleton : public QTcpServer
{
    Q_OBJECT
public:
    /**
     * @brief 获取服务单例对象的指针
     */
    static TcpServerSingleton* get_instance();
    /**
     * @brief 获取本地网络消息
     */
    void get_network_info();
    /**
     * @brief 以gui上的ip和端口号开启服务
     * @param ip
     * @param port
     */
    void open_server(QString ip, QString port);
    /**
     * @brief 关闭当前服务
     */
    void close_server();
    /**
     * @brief 根据给定的socketDescriptor，关闭对应连接的套接字
     * @param des 描述符
     */
    void close_socket(qintptr des);
    /**
     * @brief 根据给定的QtId，关闭对应连接的套接字
     * @param qtid
     */
    void close_socket(QtId qtid);

protected:
    /**
     * @brief override 有新的连接进入服务器时调用，自动为其分配描述符
     * @param des 描述符
     */
    void incomingConnection(qintptr description);
    virtual void timerEvent(QTimerEvent* e);
private:
    /**
     * @brief 根据给定的socketDescriptor，关闭对应连接的套接字
     * @param des 描述符
     */
    explicit TcpServerSingleton(QObject *parent = nullptr);
    /**
     * @brief 查找昵称
     * @param qtid 查找对象的qtid
     */
    QString get_nickname(QtId qtid);
    static TcpServerSingleton* instance;                                // Server单例的指针，防止重复创建
    QString server_ip = "127.0.0.1";                                    // Server的ip地址，默认为本机回环地址
    QHash<qintptr, ServerTcpSocket*> socket_hash;                       // 从描述符到对应socket的hash
    QHash<QtId, qintptr> descriptor_hash;                               // 从QtId到对应描述符的hash
    QHash<QtId, QString> nickname_hash;                                 // 从QtId到昵称的hash，加快查找速度
    QHash<QtId, quint16> heart_hash;                                    // 记录当前用户心跳包状态的hash
    QHash<QPair<QtId, QtId> , QList<QString>* > message_cache_hash;     // 从发送者到接收者QtId的QPair到对应离线消息的hash
    QSet<QtId> online_set;                                              // 记录在线客户端的id
    QHostAddress hostaddr;                                              // host地址
    QHostInfo hostinfo;                                                 // host信息
    int heart_timer;

signals:
    /**
     * @brief 给对应qtid发送消息
     * @param qtid 发送对象的QtId
     * @param message 发送的消息
     */
    void sig_send_message(int qtid, const QByteArray message);
    /**
     * @brief 给对应描述符发送消息
     * @param des 发送对象的socketDescriptor
     * @param message 发送的消息
     */
    void sig_send_message(qintptr des, QByteArray message);
    /**
     * @brief 发送获取本机ip地址的信号
     * @param host_info 本机的host信息
     */
    void sig_get_ip_list(QHostInfo host_info);
    /**
     * @brief 当有新用户上线时，给ui发送信号
     * @param 上线用户的QtId
     */
    void sig_online_increase(int);
    /**
     * @brief 当已在线上的用户下线时，给ui发送信号
     * @param 下线用户的QtId
     */
    void sig_online_decrease(int);
    /**
     * @brief 向ui上的log发送信息以显示
     * @param 发送的消息
     */
    void sig_update_gui(QString);
private slots:
    /**
     * @brief 接受发送信息的信号，向指定用户发送信息
     * @param qtid 发送目标的QtId
     * @param message 发送的信息
     */
    void slot_send_message_qtid(int qtid, const QByteArray message);
    /**
     * @brief 接受发送信息的信号，向指定用户发送信息
     * @param qtid 发送目标的QtId
     * @param message 发送的信息
     */
    void slot_send_message_des(qintptr qtid, const QByteArray message);
    /**
     * @brief 接受获取ip的信号，获取本机的所有ip(ipV4, ipV6, vm)
     * @param host_info 本机的host信息
     */
    void slot_get_address(QHostInfo host_info);
};

#endif // TCPSERVERSINGLETON_H
