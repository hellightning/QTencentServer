#ifndef SERVERTCPSOCKET_H
#define SERVERTCPSOCKET_H

#include <QTcpSocket>

class ServerTcpSocket : public QTcpSocket
{
public:
    explicit ServerTcpSocket(QObject *parent = nullptr);
    void close_connection();

protected:
private:
    int m_id;
signals:
    void sig_disconnected(int);
    void sig_readyRead(int, QByteArray*);
};

#endif // SERVERTCPSOCKET_H
