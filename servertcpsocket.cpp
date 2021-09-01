#include "servertcpsocket.h"

ServerTcpSocket::ServerTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &ServerTcpSocket::readyRead, [this](){
        QByteArray message;
        message = readAll();
        emit sig_readyRead(socketDescriptor(), message);
    });
    connect(this, &ServerTcpSocket::disconnected,[this](){
        qDebug() << qtid;
        if(qtid == -1){
            qDebug() << "NOT SIGNED IN";
            emit sig_disconnected_des(des);
        }else{
            qDebug() << "socket";
            emit sig_disconnected_qtid(qtid);
        }
    });
}
void ServerTcpSocket::memorize_descriptor(qintptr t_des){
    des = t_des;
    setSocketDescriptor(t_des);
}

void ServerTcpSocket::memorize_qtid(int t_id){
    qtid = t_id;
}
