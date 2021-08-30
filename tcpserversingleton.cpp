#include "tcpserversingleton.h"
#include <QDataStream>
#include <future>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

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
    QHostInfo::lookupHost(hostinfo.localHostName(), this, SLOT(slot_get_address(QHostInfo)));
    hostaddr.setAddress(server_ip);
    qDebug() << hostaddr;
}

void TcpServerSingleton::slot_get_address(QHostInfo info)
{
    emit sig_get_ip_list(info);
}

void TcpServerSingleton::open_server()
{
    /*
     * 被openServerButton调用
     * 从UI拉取ip和端口号，开始监听
     */
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

void TcpServerSingleton::slot_send_message(int qtid, const QByteArray message)
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
        QtId from_id, to_id;
        QByteArray header, chat_content, sending, receiving;
        receiving = message;
        qDebug() << receiving << message;
        QDataStream receiving_stream(&receiving, QIODevice::ReadOnly);
        QDataStream sending_stream(&sending, QIODevice::WriteOnly);
        receiving_stream >> header >> from_id >> to_id >> chat_content;
        chat_content = chat_content.trimmed();
        QPair<QtId, QtId> q_pair(from_id, to_id);
        if(header == "SEND_MESSAGE"){
            sending_stream << header;
            sending_stream << to_id;
            sending_stream << from_id;
            sending_stream << "buzai, cnm";
            slot_send_message(from_id, sending);
        }
        if(message_cache_hash.find(q_pair) == message_cache_hash.end()){
            message_cache_hash[q_pair] = new QList<QByteArray>();
        }
        // 超出缓存上限则拒绝
        if(message_cache_hash[q_pair]->length() >= 50){
            qDebug() << "Store request refused.";
        }else{
            message_cache_hash[q_pair]->append(chat_content);
            qDebug() << "Stored message: " << chat_content;
        }
    }else{
        qDebug() << "Sending message...";
        ServerTcpSocket* tmp_socket = socket_hash[descriptor_hash[qtid]];
        tmp_socket->write(message);
        qDebug() << "Sent message:" << message;
    }
}
void TcpServerSingleton::slot_send_message(qintptr des, const QByteArray message)
{
    /*
     * 服务器向指定descriptor对应的socket发送message
     */
    qDebug() << "Sending message: ";
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
        QDataStream message_stream(&message, QIODevice::ReadOnly);
        QByteArray header;
        message_stream >> header;
        qDebug() << "Header is: " << header;
        if(header.startsWith("REGISTER")){
            qDebug() << "New user requests for registering.";
            // 报文参数：昵称，密码
            QByteArray nickname;
            QByteArray password;
            message_stream >> nickname >> password;
            nickname = nickname.trimmed();
            password = password.trimmed();
            if(nickname.length() == 0 or password.length() == 0){
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                qDebug() << "New user's register failed.";
                feedback_stream << "REGISTER_FAILED";
                // 使用std::future中的async结合lambda，进行异步并发发送消息，下同
                QtConcurrent::run(QThreadPool::globalInstance(), [this](qintptr des, QByteArray feedback){
                    emit sig_send_message(des, feedback);
                }, des, feedback);
            }else{
                qDebug() << nickname;
                // 操作数据库时也使用async，异步请求
                QtConcurrent::run(QThreadPool::globalInstance(), [this](QByteArray nickname, QByteArray password, qintptr des){
                    int returned_qtid = ServerSqlSingleton::get_instance()->insert_account(nickname, password);
                    QByteArray feedback;
                    QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                    // 如果注册成功，返回的报文参数为qt号
                    if(returned_qtid != -1){
                        qDebug() << "New user(QtId=" << returned_qtid << ") registered";
                        feedback_stream << "REGISTER_SUCCEED" << returned_qtid;
                    }else{
                        qDebug() << "New user's register failed.";
                        feedback_stream << "REGISTER_FAILED";
                    }
                    emit sig_send_message(des, feedback);
                }, nickname, password, des);
            }
            // send_message(des, feedback);
        }else if(header.startsWith("SIGN_IN")){
            // 报文参数：qtid，密码
            QtId qtid = -1;
            QByteArray password;
            message_stream >> qtid >> password;
            password = password.trimmed();
            qDebug() << "Client(QtId=" << qtid << ") requests for signing in.";
            // 根据成功与否发回回应报文
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int qtid, QByteArray password, qintptr des){
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                if(ServerSqlSingleton::get_instance()->select_account(qtid, password)){
                    feedback_stream << "SIGN_IN_SUCCEED";
                    QByteArray nickname = ServerSqlSingleton::get_instance()->select_nickname(qtid).toUtf8();
                    feedback_stream << nickname;
                    qDebug() << nickname << "(QtId=" << qtid << ") signed in.";
                    descriptor_hash[qtid] = des;
                }else{
                    feedback_stream << "SIGN_IN_FAILED";
                    qDebug() << "Client(QtId=" << qtid << ") failed to signing in.";
                }
                emit sig_send_message(des, feedback);
            }, qtid, password, des);
        }else if(header.startsWith("GET_FRIEND_LIST")){
            // 报文参数：请求者qtid
            QtId qtid = -1;
            message_stream >> qtid;
            qDebug() << "Client(QtId=" << qtid << ") requests for friend list.";
            QList<QtId>* friend_list;
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int qtid, qintptr des, QList<QtId>* friend_list){
                *friend_list = ServerSqlSingleton::get_instance()->select_friends(qtid);
                // 发回报文头FRIEND_LIST，报文参数每行一个好友的qtid
                // TODO：参数也包括每个好友的nickname
                // 这个请求没有失败返回值，失败时好友列表为空
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                feedback_stream << "FRIEND_LIST";
                feedback_stream << friend_list->length();
                for(auto item : *friend_list){
                    feedback_stream << item;
                    QByteArray nickname = ServerSqlSingleton::get_instance()->select_nickname(item).toUtf8();
                }
                emit sig_send_message(des, feedback);
            }, qtid, des, friend_list);
            if(friend_list->length() >= 0){
                QtConcurrent::run(QThreadPool::globalInstance(), [this](int qtid, qintptr des, QList<QtId>* friend_list){
                    for(auto item : *friend_list){
                        QPair<QtId, QtId> q_pair(item, qtid);
                        if(message_cache_hash.find(q_pair) != message_cache_hash.end()){
                            QList<QByteArray>* t_message_list = message_cache_hash[q_pair];
                            if(t_message_list != nullptr and t_message_list->length() != 0){
                                QByteArray t_feedback;
                                QDataStream t_stream(&t_feedback, QIODevice::WriteOnly);
                                t_stream << "SEND_MESSAGE";
                                t_stream << item << qtid;
                                for(auto chat_content : *t_message_list){
                                    t_stream << chat_content;
                                }
                                emit sig_send_message(des, t_feedback);
                                delete[] t_message_list;
                                message_cache_hash.remove(q_pair);
                            }
                        }
                    }
                }, qtid, des, friend_list);
            }
        }else if(header.startsWith("ADD_FRIEND")){
            // 报文参数：请求者id，目标id
            int from_id;
            int to_id;
            message_stream >> from_id >> to_id;
            // 返回成功或失败
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int from_id, int to_id, qintptr des){
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                if(ServerSqlSingleton::get_instance()->insert_friend(from_id, to_id)){
                    feedback_stream << "ADD_FRIEND_SUCCEED" << to_id;
                }else{
                    feedback_stream << "ADD_FFIEND_FAILED";
                }
                emit sig_send_message(des, feedback);
            }, from_id, to_id, des);
        }else if(header.startsWith("SEND_MESSAGE")){
            // 报文参数：发送者id，发送对象id，发送内容
            int from_id;
            int to_id;
            QByteArray chat_content;
            message_stream >> from_id >> to_id >> chat_content;
            chat_content = chat_content.trimmed();
//            QByteArray feedback;
//            QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
//            feedback_stream << message;
            qDebug() << "Client(QtId=" << from_id << ") send to "<< "Client(QtId=" << to_id << "): " << chat_content;
            // 直接将报文原样进行转发给目标，不做额外操作
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int to_id, QByteArray feedback){
                emit sig_send_message(to_id, feedback);
            }, to_id, message);
        }else if(header.startsWith("REQUEST_MESSAGE")){
            // 报文参数：请求者id，请求对象id
            // 用来请求缓存的离线信息，待完善
            int from_id;
            int to_id;
            message_stream >> from_id >> to_id;
            // 发回格式和发送请求相同，但在一条报文中发回所有缓存消息，每行一条
            QByteArray feedback;
            QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
            feedback_stream << "SEND_MESSAGE" << to_id << from_id;
            QPair<QtId, QtId> q_pair(to_id, from_id);
            if(message_cache_hash.find(q_pair) != message_cache_hash.end()){
                feedback_stream << *message_cache_hash[q_pair];
            }
            QtConcurrent::run(QThreadPool::globalInstance(), [this](qintptr des, QByteArray feedback){
                emit sig_send_message(des, feedback);
            }, des, feedback);
        }
    });

    //
    connect(tmp_socket, &ServerTcpSocket::sig_disconnected, [this](qintptr qtid){
        close_socket(qtid);
    });

}

TcpServerSingleton::TcpServerSingleton(QObject *parent) : QTcpServer(parent)
{
    get_network_info();
    open_server();
    connect(this, SIGNAL(sig_send_message(int, const QByteArray)),
            this, SLOT(slot_send_message(int, const QByteArray)));
    if(!QMetaType::isRegistered((QMetaType::type("qintptr")))){
        qRegisterMetaType<qintptr>("qintptr");
    }
    connect(this, SIGNAL(sig_send_message(qintptr, const QByteArray)),
            this, SLOT(slot_send_message(qintptr, const QByteArray)));
//    TcpServerSingleton::qtid_distributed = INIT_QTID + ServerSqlSingleton::account_number;
}
