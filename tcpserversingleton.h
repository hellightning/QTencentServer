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
    Q_OBJECT
public:
    static TcpServerSingleton* get_instance();
    void get_network_info();
    void open_server(QString, QString);
    void close_server();
    void close_socket(qintptr);
    void close_socket(QtId);

protected:
    void incomingConnection(qintptr description);
private:
    explicit TcpServerSingleton(QObject *parent = nullptr);
    QByteArray get_nickname(QtId);
    static TcpServerSingleton* instance;
    QString server_ip = "127.0.0.1";
    QHash<qintptr, ServerTcpSocket*> socket_hash;
    QHash<QtId, qintptr> descriptor_hash;
    QHash<QtId, QByteArray> nickname_hash;
    QHash<QPair<QtId, QtId> , QList<QByteArray>* > message_cache_hash;
    ServerSqlSingleton* sql_instance;
    QHostAddress hostaddr;
    QHostInfo hostinfo;

signals:
    void sig_send_message(int, const QByteArray);
    void sig_send_message(qintptr, QByteArray);
    void sig_get_ip_list(QHostInfo);
    void sig_online_increase(int);
    void sig_online_decrease(int);
    void sig_update_gui(QString);
private slots:
    void slot_send_message_qtid(int, const QByteArray);
    void slot_send_message_des(qintptr, const QByteArray);
    void slot_get_address(QHostInfo);
};

#endif // TCPSERVERSINGLETON_H
