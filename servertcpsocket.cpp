#include "servertcpsocket.h"

ServerTcpSocket::ServerTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &ServerTcpSocket::readyRead, [this](){
        QByteArray message;
        message = readAll();
        emit sig_readyRead(socketDescriptor(), message);
    });
    connect(this, &ServerTcpSocket::disconnected,[this](){
        emit sig_disconnected(socketDescriptor());
    });
}
