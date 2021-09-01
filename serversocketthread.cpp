#include "serversocketthread.h"
#include "servertcpsocket.h"

ServerSocketThread::ServerSocketThread(QObject *parent, qintptr des) : QThread(parent)
{
    tcp_socket = new ServerTcpSocket();
    tcp_socket->setSocketDescriptor(des);
    tcp_socket->memorize_descriptor(des);
}

QByteArray ServerSocketThread::read(){
    return tcp_socket->readAll();
}

void ServerSocketThread::write(QByteArray message){
    tcp_socket->write(message);
}
