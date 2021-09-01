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
    qDebug() << qtid;
    m_qtid = qtid;
    tcp_socket->memorize_qtid(qtid);
}

void ServerSocketThread::memorize_descriptor(qintptr des)
{
    m_des = des;
    tcp_socket->memorize_descriptor(des);
}

void ServerSocketThread::close(){
    checkpoint = false;
}

void ServerSocketThread::run(){
    connect(tcp_socket, &ServerTcpSocket::sig_disconnected_des, this, &ServerSocketThread::slot_disconnected_des);
    connect(tcp_socket, &ServerTcpSocket::sig_disconnected_qtid, this, &ServerSocketThread::slot_disconnected_qtid);
    connect(tcp_socket, &ServerTcpSocket::sig_readyRead, this, &ServerSocketThread::slot_readyRead);
    tcp_socket->waitForConnected();
    while(tcp_socket->state() == QAbstractSocket::ConnectedState and checkpoint){
        QEventLoop loop;
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
    }
}

void ServerSocketThread::slot_disconnected_qtid(int qtid)
{
    emit ServerSocketThread::sig_disconnected_qtid(m_qtid);
}

void ServerSocketThread::slot_disconnected_des(qintptr des)
{
    emit ServerSocketThread::sig_disconnected_des(m_des);
}

void ServerSocketThread::slot_readyRead(qintptr des, QByteArray message)
{
    emit ServerSocketThread::sig_readyRead(des, message);
}
