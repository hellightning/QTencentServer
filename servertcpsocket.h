#ifndef SERVERTCPSOCKET_H
#define SERVERTCPSOCKET_H

#include <QTcpSocket>

class ServerTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ServerTcpSocket(QObject *parent = nullptr);
    void memorize_descriptor(qintptr);
protected:
private:
    qintptr des = -1;
signals:
    void sig_disconnected(int);
    void sig_readyRead(qintptr, QByteArray);
};

#endif // SERVERTCPSOCKET_H
