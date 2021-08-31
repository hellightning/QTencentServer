#include "mainwindow.h"
#include <QApplication>

#include "mytest.h"

TcpServerSingleton* TcpServerSingleton::instance = nullptr;

QTEST_MAIN(mytest);

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    TcpServerSingleton::get_instance();
//    ServerMainWindow w;
//    w.show();
//    return a.exec();
//}
