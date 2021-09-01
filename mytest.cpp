#include "mytest.h"
#include "mainwindow.h"
#include "tcpserversingleton.h"
#include <future>
#include <QWaitCondition>
#include <QTest>
#include <QtTest/QTest>
#include "serversqlsingleton.h"

mytest::mytest(QObject *parent) : QObject(parent)
{

}

void mytest::UiTest()
{
    ServerSqlSingleton* sql = ServerSqlSingleton::get_instance();
    QTest::qWait(1000000);
}
