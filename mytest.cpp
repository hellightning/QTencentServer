#include "mytest.h"
#include "mainwindow.h"
#include "tcpserversingleton.h"
#include <future>
#include <QWaitCondition>
#include <QTest>
#include <QtTest/QTest>

mytest::mytest(QObject *parent) : QObject(parent)
{

}

void mytest::UiTest()
{
    ServerMainWindow ui;
    ui.show();
    TcpServerSingleton* server = TcpServerSingleton::get_instance();
    server->incomingConnection(14);
    QTest::qWait(10000);
    server->close_socket(14);
    QTest::qWait(10000);
}

void mytest::OnlineAccountTest()
{

}
