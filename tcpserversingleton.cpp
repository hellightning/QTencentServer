#include "tcpserversingleton.h"
#include <QDataStream>
#include <future>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QFile>

const int INIT_QTID = 114514;
TcpServerSingleton* TcpServerSingleton::get_instance(){
    /*
     * 获取单例
     */
    QMutex instance_mutex;
    if(TcpServerSingleton::instance == nullptr){
        instance_mutex.lock();
        TcpServerSingleton::instance = new TcpServerSingleton();
        instance_mutex.unlock();
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

void TcpServerSingleton::open_server(QString ip, QString port)
{
    /*
     * 被openServerButton调用
     * 从UI拉取ip和端口号，开始监听
     */
    if(this->isListening()){
        emit sig_update_gui("Server is already listening!");
    }else if(this->listen(QHostAddress(ip), port.toUInt())){
        qDebug() << "Server listening...";
        qDebug() << "Server ip: " << ip;
        qDebug() << "Server port: " + port;
        emit sig_update_gui("Server starts listening.");
        emit sig_update_gui("Server ip  : " + ip);
        emit sig_update_gui("Server port: " + port);
    }
}

void TcpServerSingleton::close_server()
{
    /*
     * 被closeServerButton调用
     * 关闭当前监听的服务器
     */
    qDebug() << "Closing server...";
    emit sig_update_gui("Closing server...");
    this->close();
    for(auto item : online_set){
        emit sig_online_decrease(item);
    }
    qDebug() << "Server closed.";
    emit sig_update_gui("Server closed.");
}

void TcpServerSingleton::close_socket(qintptr des)
{
    mutex.lock();
    /*
     * 给定descriptor，关闭对应的socket
     */
    if(socket_hash.find(des) == socket_hash.end()){
        qDebug() << "Socket(descriptor=" << des <<") not found.";
    }else{
        qDebug() << 1;
        ServerSocketThread* tmp_socket = socket_hash[des];
        qDebug() << 2;
        tmp_socket->close();
        qDebug() << 3;
        tmp_socket->quit();
        qDebug() << 4;
//        tmp_socket->wait();

        qDebug() << 5;
        tmp_socket->deleteLater();
        qDebug() << 6;
        socket_hash.remove(des);
        qDebug() << "Socket(descriptor=" << des <<") closed.";
    }
    mutex.unlock();
}

void TcpServerSingleton::close_socket(QtId qtid)
{
    /*
     * 给定qtid,关闭对应的socket
     */
    qDebug() << 0;
    emit sig_update_gui(QString(get_nickname(qtid) + "(QtId=%1) is now offline.").arg(qtid));
    emit sig_online_decrease(qtid);
    if(descriptor_hash.find(qtid) == descriptor_hash.end()){
        qDebug() << "Client(QtId=" << qtid << ") is offline, need not to close.";
    }else{
        qDebug() << "Closing socket of client(QtId=" << qtid << ")...";
        close_socket(descriptor_hash[qtid]);
        emit sig_online_decrease(qtid);
        qDebug() << "Socket of client(QtId=" << qtid << ") closed.";
    }
}

void TcpServerSingleton::slot_send_message_qtid(int qtid, const QByteArray message)
{
    /*
     * 服务器向指定qtid发送message
     * 一般是转发消息
     */
    qDebug() << "Send message to " << qtid;
    auto des = descriptor_hash.find(qtid);
    if(online_set.find(qtid) == online_set.end() and
            (des == descriptor_hash.end()
            or (des != descriptor_hash.end()
                and socket_hash.find(descriptor_hash[qtid]) == socket_hash.end()))){
        qDebug() << "Client(QtId=" << qtid <<") is offline. Message storing...";
        QtId from_id, to_id;
        QByteArray header, sending, receiving;
        QString chat_content;
        receiving = message;
        qDebug() << receiving << message;
        QDataStream receiving_stream(&receiving, QIODevice::ReadOnly);
        QDataStream sending_stream(&sending, QIODevice::WriteOnly);
        receiving_stream >> header >> from_id >> to_id >> chat_content;
        emit sig_update_gui(get_nickname(from_id) + " send to " + get_nickname(to_id) + ": " + chat_content);
        QPair<QtId, QtId> q_pair(from_id, to_id);
        if(header.startsWith("SEND_MESSAGE") or header.startsWith("SEND_FILE")){
            sending_stream << header;
            sending_stream << to_id;
            sending_stream << from_id;
            sending_stream << QString("%1:%2 buzai,cnm").arg(QTime::currentTime().hour()).arg(QTime::currentTime().minute());
            qDebug() << "buzai...";
            emit sig_send_message(from_id, sending);
        }
        if(message_cache_hash.find(q_pair) == message_cache_hash.end()){
            message_cache_hash[q_pair] = new QList<QString>();
        }
        // 超出缓存上限则拒绝
        if(message_cache_hash[q_pair]->length() >= 50 or chat_content.startsWith("buzai, cnm") or header == "SEND_FILE"){
            qDebug() << "Store request refused.";
        }else{
            message_cache_hash[q_pair]->append(chat_content);
            qDebug() << "Stored message: " << chat_content;
        }
    }else{
        qDebug() << "Sending message...";
//        QtConcurrent::run(QThreadPool::globalInstance(), [this](QtId qtid, QByteArray message){
        ServerSocketThread* tmp_socket = socket_hash[descriptor_hash[qtid]];
        tmp_socket->write(message);
//        }, qtid, message);
        qDebug() << "Sent message:" << message;
    }
}
void TcpServerSingleton::slot_send_message_des(qintptr des, const QByteArray message)
{
    /*
     * 服务器向指定descriptor对应的socket发送message
     */
    qDebug() << "Sending message: " << message;
//    QtConcurrent::run(QThreadPool::globalInstance(), [this](qintptr des, QByteArray message){
        ServerSocketThread* tmp_socket = socket_hash[des];
        tmp_socket->write(message);
//    },des, message);
    qDebug() << "Sent message:" << message;
}

void TcpServerSingleton::incomingConnection(qintptr description)
{
    /*
     * 有新连接时调用
     * 获取或实例化socket，根据读取的信息进行对应的操作
     */
    emit sig_update_gui("New client requests for connect.");
    ServerSocketThread* tmp_socket = nullptr;
    if(socket_hash.find(description) == socket_hash.end()){
        tmp_socket = new ServerSocketThread();
        tmp_socket->start();
        tmp_socket->memorize_descriptor(description);
        socket_hash[description] = tmp_socket;
    }else{
        tmp_socket = socket_hash[description];
        tmp_socket->memorize_descriptor(description);
    }
    qDebug() << "New client requests for connexion.";
    qDebug() << "Client Descriptor: " << description;
    // 类图上的handler，用lambda处理
    connect(tmp_socket, &ServerSocketThread::sig_readyRead, [this](qintptr des, QByteArray message){
        // 用换行符/n拆分消息
        // 第一行为报文头，根据报文头进行不同操作
        QDataStream message_stream(&message, QIODevice::ReadOnly);
        QByteArray header;
        message_stream >> header;
        qDebug() << "Header is: " << header;
        if(header.startsWith("REGISTER")){
            qDebug() << "New user requests for registering.";
            emit sig_update_gui("New client requests for registering.");
            // 报文参数：昵称，密码
            QString nickname;
            QString password;
            message_stream >> nickname >> password;
            nickname = nickname.trimmed();
            password = password.trimmed();
            if(nickname.length() == 0 or password.length() <= 4){
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                qDebug() << "New user's register failed.";
                emit sig_update_gui("Client(nickname=" + nickname + ") register failed.");
                feedback_stream << "REGISTER_FAILED";
                // 使用std::future中的async结合lambda，进行异步并发发送消息，下同
                QtConcurrent::run(QThreadPool::globalInstance(), [this](qintptr des, QByteArray feedback){
                    emit sig_send_message(des, feedback);
                }, des, feedback);
            }else{
                qDebug() << nickname;
                // 操作数据库时也使用async，异步请求
                QtConcurrent::run(QThreadPool::globalInstance(), [this](QString nickname, QString password, qintptr des){
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
                    emit sig_update_gui(QString(nickname+"(QtId=%1) register succeed.").arg(returned_qtid));
                }, nickname, password, des);
            }
            // send_message(des, feedback);
        }else if(header.startsWith("SIGN_IN")){
            // 报文参数：qtid，密码
            QtId qtid = -1;
            QString password;
            message_stream >> qtid >> password;
            qDebug() << qtid << password;
            password = password.trimmed();
            qDebug() << "Client(QtId=" << qtid << ") requests for signing in.";
            // 根据成功与否发回回应报文
            auto fut_reg = QtConcurrent::run(QThreadPool::globalInstance(), [this](int qtid, QString password, qintptr des){
                qDebug() << qtid << password;
                bool state = false;
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                if(ServerSqlSingleton::get_instance()->select_account(qtid, password)){
                    feedback_stream << "SIGN_IN_SUCCEED";
                    QString nickname = ServerSqlSingleton::get_instance()->select_nickname(qtid).toUtf8();
                    feedback_stream << nickname;
                    descriptor_hash[qtid] = des;
                    qDebug() << nickname << "(QtId=" << qtid << ") signed in.";
                    nickname_hash[qtid] = nickname;
                    emit sig_online_increase(qtid);
                    emit sig_update_gui(QString(nickname + "(QtId=%1) is now online.").arg(qtid));
                    state = true;
                }else{
                    feedback_stream << "SIGN_IN_FAILED";
                    qDebug() << "Client(QtId=" << qtid << ") failed to signing in.";
                }
                emit sig_send_message(des, feedback);
                return state;
            }, qtid, password, des);
            auto suc = fut_reg.result();
            if(suc){
                if(socket_hash.find(des) != socket_hash.end()){
                    heart_hash[qtid] = 0;
                    socket_hash[des]->memorize_qtid(qtid);
                    descriptor_hash[qtid] = des;
                    qDebug() << qtid;
                    online_set.insert(qtid);
                }
            }
        }else if(header.startsWith("GET_FRIEND_LIST")){
            // 报文参数：请求者qtid
            QtId qtid = -1;
            message_stream >> qtid;
            qDebug() << "Client(QtId=" << qtid << ") requests for friend list.";
            auto fut_friend = QtConcurrent::run(QThreadPool::globalInstance(), [this](int qtid, qintptr des){
                auto friend_list = (ServerSqlSingleton::get_instance()->select_friends(qtid));
                // 发回报文头FRIEND_LIST，报文参数每行一个好友的qtid
                // TODO：参数也包括每个好友的nickname
                // 这个请求没有失败返回值，失败时好友列表为空
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                feedback_stream << "FRIEND_LIST";
                feedback_stream << friend_list.length();
                for(int item : friend_list){
                    feedback_stream << item;
                    QString nickname = ServerSqlSingleton::get_instance()->select_nickname(item);
                    feedback_stream << nickname;
                    QPair<QtId, QtId> q_pair(item, qtid);
                    if(message_cache_hash.find(q_pair) == message_cache_hash.end()
                            or message_cache_hash[q_pair]->length() <= 0){
                        feedback_stream << 0;
                    }else{
                        auto f_message = message_cache_hash[q_pair];
                        feedback_stream << f_message->length();
                        for(auto item : *f_message){
                            feedback_stream << item;
                       }
                    }
                }
                emit sig_send_message(des, feedback);
            }, qtid, des);
        }else if(header.startsWith("ADD_FRIEND")){
            // 报文参数：请求者id，目标id
            int from_id;
            int to_id;
            message_stream >> from_id >> to_id;
            // 返回成功或失败
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int from_id, int to_id, qintptr des){
                QByteArray feedback;
                QDataStream feedback_stream(&feedback, QIODevice::WriteOnly);
                if(from_id != to_id and (ServerSqlSingleton::get_instance()->insert_friend(from_id, to_id)
                        and ServerSqlSingleton::get_instance()->insert_friend(to_id, from_id))){
                    qDebug() << instance->get_nickname(to_id);
                    feedback_stream << "ADD_FRIEND_SUCCEED" << to_id << instance->get_nickname(to_id);
                    emit sig_update_gui(get_nickname(from_id) + " add " + get_nickname(to_id) + " as new friend.");
                    if(descriptor_hash.find(to_id) != descriptor_hash.end()){
                        QByteArray t_feedback;
                        QDataStream t_stream(&t_feedback, QIODevice::WriteOnly);
                        t_stream << "ADD_FRIEND_SUCCEED" << from_id << instance->get_nickname(from_id);
                        emit sig_send_message(descriptor_hash[to_id], t_feedback);
                    }
                }else{
                    feedback_stream << "ADD_FFIEND_FAILED";
                }
                emit sig_send_message(des, feedback);
            }, from_id, to_id, des);
        }else if(header.startsWith("SEND_MESSAGE")){
            // 报文参数：发送者id，发送对象id，发送内容
            int from_id;
            int to_id;
            QString chat_content;
            message_stream >> from_id >> to_id >> chat_content;
            qDebug() << "Client(QtId=" << from_id << ") send to "<< "Client(QtId=" << to_id << "): " << chat_content;
            // 直接将报文原样进行转发给目标，不做额外操作
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int to_id, QByteArray feedback){
                emit sig_send_message(to_id, feedback);
            }, to_id, message);
        }else if(header.startsWith("SEND_FILE")){
            // 报文参数：发送者id，发送对象id，文件大小，文件类型（后缀名），文件名称，文件数据
            int from_id;
            int to_id;
            unsigned long long file_size;
            QString file_type;
            QString file_name;
            QByteArray file_byte;
            message_stream >> from_id >> to_id >> file_size >> file_type >> file_name >> file_byte;
            if((unsigned long long)file_byte.size() >= (1<<31) * sizeof(qint64)){
                qDebug() << "Client(QtId=" << from_id << ") try to send large file.";
                QByteArray large_feedback;
                QDataStream l_stream(&large_feedback, QIODevice::WriteOnly);
                l_stream << "SEND_MESSAGE" << to_id << from_id << "too large to receive!";
                emit sig_send_message(from_id, large_feedback);
            }else{
                qDebug() << "Client(QtId=" << from_id << ") send file " << file_name+"."+file_type << " to Client(QtId=" << to_id << ").";
                emit sig_update_gui(QString("Client(QtId=%1) send file " + file_name + "." + file_type + " to Client(QtId=%2).").arg(from_id).arg(to_id));
                // 直接将报文原样进行转发给目标，不做额外操作
                emit sig_send_message(to_id, message);

            }
        }else if(header.startsWith("SEND_GROUP_MESSAGE")){
            // 报文参数: 发送者id，群聊id，群聊内容
            int from_id;
            int group_id;
            QString chat_content;
            message_stream >> from_id >> group_id >> chat_content;
            QtConcurrent::run(QThreadPool::globalInstance(), [this](int from_id, int group_id, QString chat_content){
                // TODO: 从db获取群号对应的群员名单
                auto group_list = QList<QtId>();//ServerSqlSingleton::get_instance()->select_grouper(group_id);
                for(auto item : group_list){

                    if(item != from_id){
                        QByteArray feedback;
                        QDataStream f_stream(&feedback, QIODevice::WriteOnly);
                        f_stream << "SEND_GROUP_MESSAGE" << group_id << from_id << item << chat_content;
                        emit sig_send_message(item, feedback);
                    }
                }
            }, from_id, group_id, chat_content);


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
        }else if(header.startsWith("HEART_BEAT")){
            emit sig_send_message(des, QByteArray("HEART_BREAK"));
            QtId qtid;
            qDebug() << QString("%1 : %2").arg(QTime::currentTime().minute()).arg(QTime::currentTime().second());
            message_stream >> qtid;
            qDebug() << "HERERERE" << qtid;
            mutex.lock();
            heart_hash[qtid] = 0;
            mutex.unlock();
        }
    });

    //
    connect(tmp_socket, &ServerSocketThread::sig_disconnected_des, [this](qintptr des){
        close_socket(des);
    });
    connect(tmp_socket, &ServerSocketThread::sig_disconnected_qtid, [this](QtId qtid){
        close_socket(qtid);
    });
}

TcpServerSingleton::TcpServerSingleton(QObject *parent) : QTcpServer(parent)
{
    get_network_info();
    connect(this, SIGNAL(sig_send_message(int, const QByteArray)),
            this, SLOT(slot_send_message_qtid(int, const QByteArray)));
    if(!QMetaType::isRegistered((QMetaType::type("qintptr")))){
        qRegisterMetaType<qintptr>("qintptr");
    }
    connect(this, SIGNAL(sig_send_message(qintptr, const QByteArray)),
            this, SLOT(slot_send_message_des(qintptr, const QByteArray)));
    //    TcpServerSingleton::qtid_distributed = INIT_QTID + ServerSqlSingleton::account_number;
    heart_timer = startTimer(10000);
}

QString TcpServerSingleton::get_nickname(QtId qtid)
{
    if(nickname_hash.find(qtid) == nickname_hash.end()){
//        QtConcurrent::run(QThreadPool::globalInstance(), [this](QtId qtid){
            nickname_hash[qtid] = ServerSqlSingleton::get_instance()->select_nickname(qtid);
//        }, qtid);
    }
    return nickname_hash[qtid];
}

void TcpServerSingleton::timerEvent(QTimerEvent *e)
{

    if(e->timerId() == heart_timer){
        QList<int> lst;
        qDebug() << heart_hash;
        for(auto item : online_set){
            qDebug() << item << heart_hash;
            if(heart_hash.find(item) == heart_hash.end()){
                qDebug() << "UNSIGNED";
            }else if(heart_hash[item] >= 3){
                lst.append(item);
                emit sig_update_gui(get_nickname(item) + "is now offline.");
                close_socket(item);
            }else{
                mutex.lock();
                heart_hash[item] += 1;
                mutex.unlock();
            }
        }
        for (auto e : lst) {
            online_set.remove(e);
        }
    }
}
