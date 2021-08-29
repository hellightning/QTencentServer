#include "mainwindow.h"
#include <QApplication>

TcpServerSingleton* TcpServerSingleton::instance = nullptr;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServerSingleton::get_instance();
    ServerMainWindow w;
    w.show();
    return a.exec();
}
