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
    for (int i=0;i<20000; i++) {
        QString qs = QString("$%1").arg(i);
        QByteArray qb = qs.toLatin1();
//        ui.add_backlog_list(qs);
    }
    QTest::qWait(1000000);
}

void mytest::OnlineAccountTest()
{

}
