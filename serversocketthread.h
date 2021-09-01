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
    explicit ServerSocketThread(QObject *parent = nullptr, qintptr des = -1);
    ServerSocketThread(qintptr des);
    QByteArray read();
    void write(QByteArray message);
    void memorize_qtid(int qtid);
    void memorize_descriptor(qintptr des);
    void close();
protected:
    virtual void run();
private:
    int m_qtid = -1;
    qintptr m_des = -1;
    ServerTcpSocket* tcp_socket;
private slots:
    void slot_disconnected_qtid(int qtid);
    void slot_disconnected_des(qintptr des);
    void slot_readyRead(qintptr des, QByteArray message);
signals:
    /**
     * @brief 发送socket断开连接的信号
     * @param qtid 断开连接的socket连接用户的QtId
     */
    void sig_disconnected_qtid(int qtid);
    /**
     * @brief 发送socket断开连接的信号
     * @param des 断开连接的socket的描述符
     */
    void sig_disconnected_des(qintptr des);
    /**
     * @brief 发送收到信息的信号，以开始读信息
     * @param des 收到信号的socket的描述符
     * @param message 收到的信息
     */
    void sig_readyRead(qintptr des, QByteArray message);
};

#endif // SERVERSOCKETTHREAD_H
