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

void ServerSocketThread::memorize_qtid(int qtid)
{
    m_qtid = qtid;
}

void ServerSocketThread::memorize_descriptor(qintptr des)
{
    m_des = des;
    tcp_socket->setSocketDescriptor(des);
}

void ServerSocketThread::close(){
    tcp_socket->close();
}

void ServerSocketThread::run(){
    connect(tcp_socket, &ServerTcpSocket::sig_disconnected_des, this, &ServerSocketThread::slot_disconnected_des);
    connect(tcp_socket, &ServerTcpSocket::sig_disconnected_qtid, this, &ServerSocketThread::slot_disconnected_qtid);
    connect(tcp_socket, &ServerTcpSocket::sig_readyRead, this, &ServerSocketThread::slot_readyRead);
    tcp_socket->waitForConnected();
    while(tcp_socket->state() == QAbstractSocket::ConnectedState){
        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();
    }
}

void ServerSocketThread::slot_disconnected_qtid(int qtid)
{
    emit ServerSocketThread::sig_disconnected_qtid(qtid);
}

void ServerSocketThread::slot_disconnected_des(qintptr des)
{
    emit ServerSocketThread::slot_disconnected_des(des);
}

void ServerSocketThread::slot_readyRead(qintptr des, QByteArray message)
{
    emit ServerSocketThread::sig_readyRead(des, message);
}
