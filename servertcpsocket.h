#ifndef SERVERTCPSOCKET_H
#define SERVERTCPSOCKET_H

#include <QTcpSocket>

class ServerTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ServerTcpSocket(QObject *parent = nullptr);
    void memorize_descriptor(qintptr);
    void memorize_qtid(int);
protected:
private:
    qintptr des = -1;
    int qtid = -1;
signals:
    void sig_disconnected_qtid(int);
    void sig_disconnected_des(qintptr);
    void sig_readyRead(qintptr, QByteArray);
};

#endif // SERVERTCPSOCKET_H
