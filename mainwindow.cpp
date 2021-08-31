#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "servertcpsocket.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QtMath>
#include <QWidget>
#include <QPaintEvent>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TcpServerSingleton* tcp_server = TcpServerSingleton::get_instance();
    connect(tcp_server, SIGNAL(sig_get_ip_list(QHostInfo)), this, SLOT(slot_get_ip_list(QHostInfo)));
    connect(tcp_server, SIGNAL(sig_online_increase(int)), this, SLOT(on_online_increase(int)));
    connect(tcp_server, SIGNAL(sig_online_decrease(int)), this, SLOT(on_online_decrease(int)));
//    需要请直接调用静态方法get_instance()，指针不作为MainWindow的成员

    setWindowFlag(Qt::FramelessWindowHint);
    auto shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(8);
    ui->listWidget->setGraphicsEffect(shadow_effect);
    auto eff2 = new QGraphicsDropShadowEffect(this);
    eff2->setOffset(0, 0);
    eff2->setColor(Qt::gray);
    eff2->setBlurRadius(8);
    ui->listWidget_2->setGraphicsEffect(eff2);
}

void ServerMainWindow::paintEvent(QPaintEvent *event) {
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}

void ServerMainWindow::add_backlog_list()
{

}

void ServerMainWindow::clean_backlog_list()
{

}


void ServerMainWindow::on_closeServerButton_clicked()
{
    TcpServerSingleton* instance = TcpServerSingleton::get_instance();
    instance->close_server();
}


void ServerMainWindow::on_openServerButton_clicked()
{
    TcpServerSingleton* instance = TcpServerSingleton::get_instance();
    instance->open_server(ui->comboBox->currentText(), ui->lineEdit_2->text());
}

void ServerMainWindow::on_online_increase(int id)
{
    ui->listWidget->addItem(QString("%1").arg(id));
}

void ServerMainWindow::on_online_decrease(int id)
{
    for (int i=0;i<ui->listWidget->count();i++) {
        if (ui->listWidget->item(i)->text().toInt() == id) {
            QListWidgetItem* item = ui->listWidget->takeItem(i);
            delete item;
        }
    }
}

void ServerMainWindow::slot_get_ip_list(QHostInfo info)
{
    if (info.addresses().count() > 0) {
        qDebug()<<info.addresses().count();
        for (int i = 0; i < info.addresses().count(); i++) {
            qDebug()<<info.addresses().at(i);
            ui->comboBox->addItem(info.addresses().at(i).toString());
        }
    }
}
