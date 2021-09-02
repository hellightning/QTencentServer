/**
 * @file serversocketthread.h
 * @brief 封装套接字的线程类
 */
#ifndef SERVERSOCKETTHREAD_H
#define SERVERSOCKETTHREAD_H

#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include "servertcpsocket.h"

class ServerSocketThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief ServerSocketThread 构造函数
     * @param parent
     * @param des 描述符
     */
    explicit ServerSocketThread(QObject *parent = nullptr, qintptr des = -1);
    ServerSocketThread(qintptr des);
    /**
     * @brief read 获取封装的socket读取到的二进制串
     */
    QByteArray read();
    /**
     * @brief write 用封装的socket向客户端写消息
     * @param message 向客户端发送的消息
     */
    void write(QByteArray message);
    /**
     * @brief memorize_qtid 记录本线程对应客户端的QtId，并传给socket
     * @param qtid 记录的QtId
     */
    void memorize_qtid(int qtid);
    /**
     * @brief memorize_descriptor 记录本线程socket的描述符，并传给socket且赋值
     * @param des 收到信号的socket的描述符
     */
    void memorize_descriptor(qintptr des);
    /**
     * @brief close 关闭本线程封装的socket
     */
    void close();
    /**
     * @brief state 返回tcp_socket的连接状态
     * @return
     */
    QAbstractSocket::SocketState state();
protected:
    /**
     * @brief run 重写继承自QThread类的run，在套接字保持连接时死循环
     */
    virtual void run();
private:
    int m_qtid = -1;            // 保存的qtid
    qintptr m_des = -1;         // 保存的socket描述符
    ServerTcpSocket* tcp_socket;// 封装的自定义socket对象的指针
    bool checkpoint = true;     // 标记循环终止
private slots:
    /**
     * @brief slot_disconnected_qtid 接受socket断开的信号并转发
     * @param qtid 断开连接的socket连接用户的QtId
     */
    void slot_disconnected_qtid(int qtid);
    /**
     * @brief slot_disconnected_des 接受socket断开的信号并转发
     * @param des 断开连接的socket的描述符
     */
    void slot_disconnected_des(qintptr des);
    /**
     * @brief slot_readyread 接受socket收到信息的信号并转发
     * @param des 收到信号的socket的描述符
     * @param message 收到的信息
     */
    void slot_readyRead(qintptr des, QByteArray message);
signals:
    /**
     * @brief sig_disconnected_qtid 发送socket断开连接的信号
     * @param qtid 断开连接的socket连接用户的QtId
     */
    void sig_disconnected_qtid(int qtid);
    /**
     * @brief sig_disconnected_des 发送socket断开连接的信号
     * @param des 断开连接的socket的描述符
     */
    void sig_disconnected_des(qintptr des);
    /**
     * @brief sig_readyRead 发送收到信息的信号，以开始读信息
     * @param des 收到信号的socket的描述符
     * @param message 收到的信息
     */
    void sig_readyRead(qintptr des, QByteArray message);
};

#endif // SERVERSOCKETTHREAD_H
