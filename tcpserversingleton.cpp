#include "tcpserversingleton.h"

void TcpServerSingleton::get_network_info()
{

}

void TcpServerSingleton::open_server()
{
    /*
     * 被openServerButton调用
     * 从UI拉取ip和端口号，开始监听
     */
    hostaddr.setAddress(server_ip);
    qDebug() << hostaddr;
    if(this->listen(hostaddr, port.toUInt())){
        qDebug() << "Server Listening...";
        qDebug() << "Server ip: " << hostaddr;
        qDebug() << "Server port: " + port;
    }
}

void TcpServerSingleton::close_server()
{
    /*
     * 被closeServerButton调用
     * 关闭当前监听的服务器
     */
    this->close();
}

void TcpServerSingleton::close_socket(qintptr des)
{
    /*
     * 给定descriptor，关闭对应的socket
     */
    if(socket_hash.find(des) == socket_hash.end()){
        qDebug() << "Socket(descriptor=" << des <<") not found.";
    }else{
        ServerTcpSocket* tmp_socket = socket_hash[des];
        tmp_socket->close_connection();
        delete tmp_socket;
        socket_hash.remove(des);
        qDebug() << "Socket(descriptor=" << des <<") closed.";
    }
}

void TcpServerSingleton::close_socket(QtId qtid)
{
    /*
     * 给定qtid,关闭对应的socket
     */
    if(descriptor_hash.find(qtid) == descriptor_hash.end()){
        qDebug() << "Client(QtId=" << qtid << ") is offline, need not to close.";
    }else{
        qDebug() << "Closing socket of client(QtId=" << qtid << ")...";
        close_socket(descriptor_hash[qtid]);
        qDebug() << "Socket of client(QtId=" << qtid << ") closed.";
    }
}

void TcpServerSingleton::ready_read(int, const QByteArray)
{

}

void TcpServerSingleton::send_message(int qtid, const QByteArray message)
{
    /*
     * 服务器向指定qtid发送message
     * 一般是转发消息
     */
    auto des = descriptor_hash.find(qtid);
    if(des == descriptor_hash.end()
            or (des != descriptor_hash.end()
                and socket_hash.find(descriptor_hash[qtid]) == socket_hash.end())){
        qDebug() << "Client(QtId=" << qtid <<") is offline. Message storing...";
        if(message_cache_hash.find(qtid) == message_cache_hash.end()){
            message_cache_hash[qtid] = new QList<QByteArray>();
        }
        // 超出缓存上限则拒绝
        if(message_cache_hash[qtid]->length() >= 50){
            qDebug() << "Store request refused.";
        }else{
            message_cache_hash[qtid]->append(message);
            qDebug() << "Stored message: " << message;
        }
    }else{
        qDebug() << "Sending message...";
        ServerTcpSocket* tmp_socket = socket_hash[descriptor_hash[qtid]];
        tmp_socket->write(message);
        qDebug() << "Sent message:" << message;

    }
}

void TcpServerSingleton::incomingConnection(qintptr description)
{
    /*
     * 有新连接时调用
     *
     */
    ServerTcpSocket* tmp_socket = nullptr;
    if(socket_hash.find(description) == socket_hash.end()){
        tmp_socket = new ServerTcpSocket();
        tmp_socket->setSocketDescriptor(description);
        socket_hash[description] = tmp_socket;

    }else{
        tmp_socket = socket_hash[description];
    }

    qDebug() << "New client requests for connexion.";
    qDebug() << "Client Descriptor: " << tmp_socket->socketDescriptor();

    // 架构图上的handler
    connect(tmp_socket, &ServerTcpSocket::sig_readyRead, [this](int qtid, QByteArray message){

    });

    //
    connect(tmp_socket, &ServerTcpSocket::sig_disconnected, [this](int qtid){

    });

}

TcpServerSingleton::TcpServerSingleton(QObject *parent) : QTcpServer(parent)
{

}
