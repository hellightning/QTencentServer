/** @file servertcpsocket.h
 * @brief 服务端socket的类
 * @details 封装了服务端需要的一些方法和信号
 */
#ifndef SERVERTCPSOCKET_H
#define SERVERTCPSOCKET_H

#include <QTcpSocket>
/**
 * @brief The ServerTcpSocket class
 */
class ServerTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ServerTcpSocket(QObject *parent = nullptr);
    /**
     * @brief 在登陆时记录该TcpSocket的descriptor
     * @param des 记录的描述符
     */
    void memorize_descriptor(qintptr des);
    /**
     * @brief 登陆成功时记录该TcpSocket对应的QtId
     * @param qtid socket所连接的用户的QtId
     */
    void memorize_qtid(int qtid);
protected:
private:
    qintptr des = -1;                           // 记录的descriptor
    int qtid = -1;                              // 连接用户的QtId
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

#endif // SERVERTCPSOCKET_H
