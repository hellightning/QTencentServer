#ifndef TCPSERVERSINGLETON_H
#define TCPSERVERSINGLETON_H

#include <QTcpServer>
#include <QHostAddress>
#include <QHostInfo>
#include "servertcpsocket.h"
#include "serversqlsingleton.h"

typedef int QtId;
typedef QString Message;
class TcpServerSingleton : public QTcpServer
{
public:
    static TcpServerSingleton* get_instance();
    void get_network_info();
    void open_server();
    void close_server();
    void close_socket(qintptr);
    void close_socket(QtId);
    void send_message(int, const QByteArray);
    void send_message(qintptr, const QByteArray);
protected:
    void incomingConnection(qintptr description);
private:
    explicit TcpServerSingleton(QObject *parent = nullptr);
    static TcpServerSingleton* instance;
    QString server_ip = "127.0.0.1";
    QString port = "2333";
    QHash<qintptr, ServerTcpSocket*> socket_hash;
    QHash<QtId, qintptr> descriptor_hash;
    QHash<QPair<QtId, QtId> , QList<QByteArray>* > message_cache_hash;
    ServerSqlSingleton* sql_instance;
    QHostAddress hostaddr;

signals:
    void sig_readyRead(qintptr, QByteArray*);
    void sig_disconnect(qintptr);
private slots:
    void slot_close_socket(qintptr);
};

#endif // TCPSERVERSINGLETON_H
