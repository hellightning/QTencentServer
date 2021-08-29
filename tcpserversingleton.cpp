#include "tcpserversingleton.h"

const int INIT_QTID = 114514;
TcpServerSingleton* TcpServerSingleton::get_instance(){
    /*
     * 获取单例
     */
    if(TcpServerSingleton::instance == nullptr){
        TcpServerSingleton::instance = new TcpServerSingleton();
    }
    return TcpServerSingleton::instance;
}

void TcpServerSingleton::get_network_info()
{
    /*
     * 获取默认网络信息，将本机ip作为地址
     */
    hostaddr = QHostInfo::localHostName();
    qDebug() << hostaddr;
}

void TcpServerSingleton::open_server()
{
    /*
     * 被openServerButton调用
     * 从UI拉取ip和端口号，开始监听
     */
    get_network_info();
    hostaddr.setAddress(server_ip);
    qDebug() << hostaddr;
    if(this->listen(hostaddr, port.toUInt())){
        qDebug() << "Server listening...";
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
    qDebug() << "Closing server...";
    this->close();
    qDebug() << "Server closed.";
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
void TcpServerSingleton::send_message(qintptr des, const QByteArray message)
{
    /*
     * 服务器向指定descriptor对应的socket发送message
     * 一般是转发消息
     */
    qDebug() << "Sending message...";
    ServerTcpSocket* tmp_socket = socket_hash[des];
    tmp_socket->write(message);
    qDebug() << "Sent message:" << message;
}

void TcpServerSingleton::incomingConnection(qintptr description)
{
    /*
     * 有新连接时调用
     * 获取或实例化socket，根据读取的信息进行对应的操作
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

    // 类图上的handler，用lambda处理
    connect(tmp_socket, &ServerTcpSocket::sig_readyRead, [this](qintptr des, QByteArray message){
        // 用换行符/n拆分消息
        // 第一行为报文头，根据报文头进行不同操作
        QList<QByteArray> contents = parse_message(message);
        if(contents[0] == "REGISTER"){
            // 报文参数：昵称，密码
            QByteArray nickname = contents[1];
            QByteArray password = contents[2];
            // 操作数据库
            int returned_qtid = ServerSqlSingleton::get_instance()->insert_account(nickname, password);
            // 如果注册成功，返回的报文参数为qt号
            if(returned_qtid != -1){
                QString send_back_message = "REGISTER_SUCCEED\n" + QString(returned_qtid);
                send_message(des, send_back_message.toUtf8());
            }else{
                send_message(des, "REGISTER_FAILED");
            }

        }else if(contents[0] == "SIGN_IN"){
            // 报文参数：qtid，密码
            QtId qtid = contents[1].toInt();
            QByteArray password = contents[2];
            // 根据成功与否发回回应报文
            if(ServerSqlSingleton::get_instance()->select_account(qtid, password)){
                send_message(des, "SIGN_IN_SUCCEED");
            }else{
                send_message(des, "SIGN_IN_FAILED");
            }
        }else if(contents[0] == "GET_FRIEND_LIST"){
            // 报文参数：请求者qtid
            QtId qtid = contents[1].toInt();
            QList<QtId> friend_list = ServerSqlSingleton::get_instance()->select_friends(qtid);
            // 发回报文头FRIEND_LIST，报文参数每行一个好友的qtid
            // TODO：参数也包括每个好友的nickname
            // 这个请求没有失败返回值，失败时好友列表为空
            QByteArray send_back_message = "FRIEND_LIST\n";
            for(auto item : friend_list){
                send_back_message += item + '\n';
            }
            send_message(des, send_back_message);
            ;
        }else if(contents[0] == "ADD_FRIEND"){
            // 报文参数：请求者id，目标id
            QByteArray from_id = contents[1];
            QByteArray to_id = contents[2];
            // 返回成功或失败
            if(ServerSqlSingleton::get_instance()->insert_friend(from_id.toInt(), to_id.toInt())){
                send_message(des, "ADD_FRIEND_SUCCEED\n" + to_id + "\n");
            }else{
                send_message(des, "ADD_FRIEND_FAILED\n");
            }
        }else if(contents[0] == "SEND_MESSAGE"){
            // 报文参数：发送者id，发送对象id，发送内容
            int from_id = contents[1].toInt();
            int to_id = contents[2].toInt();
            QByteArray chat_content = contents[3];
            // 直接将报文原样进行转发给目标，不做额外操作
            send_message(to_id, message);
        }else if(contents[0] == "REQUEST_MESSAGE"){
            // 报文参数：请求者id，请求对象id
            // 用来请求缓存的离线信息，待完善
            int from_id = contents[1].toInt();
            int to_id = contents[2].toInt();
            // 发回格式和发送请求相同，但在一条报文中发回所有缓存消息，每行一条
            QByteArray send_back_message = "SEND_MESSAGE\n";
            send_back_message += contents[2] + '\n';
            send_back_message += contents[1] + '\n';
            if(message_cache_hash.find(from_id) != message_cache_hash.end()){
                for(auto item : *message_cache_hash[from_id]){
                    auto parsed_item = TcpServerSingleton::parse_message(item);
                    if(parsed_item[1].toInt() == to_id){
                        send_back_message += parsed_item[3];
                    }
                }
            }
            send_message(des, send_back_message);
        }
    });

    //
    connect(tmp_socket, &ServerTcpSocket::sig_disconnected, [this](int qtid){
        close_socket(qtid);
    });

}

TcpServerSingleton::TcpServerSingleton(QObject *parent) : QTcpServer(parent)
{
    get_network_info();
//    TcpServerSingleton::qtid_distributed = INIT_QTID + ServerSqlSingleton::account_number;
}

QList<QByteArray> TcpServerSingleton::parse_message(QByteArray message)
{
    return message.split('\n');
}
