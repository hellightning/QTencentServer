#include "mainwindow.h"
#include "ui_mainwindow.h"

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcp_server = TcpServerSingleton::get_instance();
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}

QString ServerMainWindow::get_ip()
{

}

QString ServerMainWindow::get_port()
{

}

void ServerMainWindow::update_online_list()
{

}

void ServerMainWindow::add_backlog_list()
{

}

void ServerMainWindow::clean_backlog_list()
{

}


void ServerMainWindow::on_closeServerButton_clicked()
{

}


void ServerMainWindow::on_openServerButton_clicked()
{

}

void ServerMainWindow::on_online_increase()
{

}

void ServerMainWindow::on_online_decrease()
{

}

