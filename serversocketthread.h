#ifndef SERVERSOCKETTHREAD_H
#define SERVERSOCKETTHREAD_H

#include <QThread>
#include "servertcpsocket.h"

class ServerSocketThread : public QThread
{
public:
    explicit ServerSocketThread(QObject *parent = nullptr, qintptr des = -1);
    ServerSocketThread(qintptr des);
    QByteArray read();
    void write(QByteArray message);
private:
    ServerTcpSocket* tcp_socket;
private slots:
    ;
signals:
    ;
};

#endif // SERVERSOCKETTHREAD_H
