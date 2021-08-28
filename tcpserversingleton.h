#ifndef TCPSERVERSINGLETON_H
#define TCPSERVERSINGLETON_H

#include <QTcpServer>
#include "servertcpsocket.h"
#include "serversqlsingleton.h"

class TcpServerSingleton : public QTcpServer
{
public:
    static TcpServerSingleton* get_instance();
    void get_network_info();
    void open_server();
    void close_server();
    void close_socket(qintptr);
    void ready_read(int, const QByteArray);
    void send_message(int, const QByteArray);
protected:
    void incomingConnection(qintptr description);
private:
    explicit TcpServerSingleton(QObject *parent = nullptr);
    static TcpServerSingleton* instance;
    QHash<qintptr, ServerTcpSocket*> socket_hash;
    ServerSqlSingleton* sql_instance;

signals:
    void sig_connected(qintptr, QString);
    void sig_readyRead(int, QByteArray*);
private slots:

};

inline TcpServerSingleton *TcpServerSingleton::get_instance(){
    if(instance == nullptr){
        instance = new TcpServerSingleton();
    }
    return instance;
}

#endif // TCPSERVERSINGLETON_H
