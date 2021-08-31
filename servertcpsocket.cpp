#include "servertcpsocket.h"

ServerTcpSocket::ServerTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &ServerTcpSocket::readyRead, [this](){
        QByteArray message;
        message = readAll();
        emit sig_readyRead(socketDescriptor(), message);
    });
    connect(this, &ServerTcpSocket::disconnected,[this](){
        if(qtid == -1){
            emit sig_disconnected_des(des);
        }else{
            emit sig_disconnected_qtid(qtid);
        }
    });
}
void ServerTcpSocket::memorize_descriptor(qintptr t_des){
    des = t_des;
}

void ServerTcpSocket::memorize_qtid(int t_id){
    qtid = t_id;
}
