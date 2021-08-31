#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "servertcpsocket.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QtMath>
#include <QWidget>
#include <QPaintEvent>
#include <QMimeData>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TcpServerSingleton* tcp_server = TcpServerSingleton::get_instance();
    connect(tcp_server, SIGNAL(sig_get_ip_list(QHostInfo)), this, SLOT(slot_get_ip_list(QHostInfo)));
    connect(tcp_server, SIGNAL(sig_online_increase(int)), this, SLOT(on_online_increase(int)));
    connect(tcp_server, SIGNAL(sig_online_decrease(int)), this, SLOT(on_online_decrease(int)));
    connect(tcp_server, SIGNAL(sig_update_gui(QString)), this, SLOT(add_backlog_list(QString)));
//    需要请直接调用静态方法get_instance()，指针不作为MainWindow的成员
    setAcceptDrops(true);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
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
    auto shadow_effect3 = new QGraphicsDropShadowEffect(this);
    shadow_effect3->setOffset(0, 0);
    shadow_effect3->setColor(Qt::black);
    shadow_effect3->setBlurRadius(20);
    ui->MainFrame->setGraphicsEffect(shadow_effect3);
}

void ServerMainWindow::paintEvent(QPaintEvent *event) {
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(-10, -10, this->width()+20, this->height()+20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(-i, -i, this->width()-(-i)*2, this->height()-(-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}

void ServerMainWindow::add_backlog_list(QString msg)
{

    ui->listWidget_2->addItem(msg);
    if (ui->listWidget_2->count() > 1000)
        this->clean_backlog_list();
}

void ServerMainWindow::clean_backlog_list()
{
    for (int i=0; i<500; i++) {
        QListWidgetItem* item = ui->listWidget_2->takeItem(0);
        delete item;
    }
}

void ServerMainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        isDrag = true;
        mouse_start_point = e->globalPos();
        window_topleft_point = frameGeometry().topLeft();
    }
}

void ServerMainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        isDrag = false;
    }
}

void ServerMainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (isDrag) {
        QPoint dist = e->globalPos() - mouse_start_point;
        this->move(window_topleft_point + dist);
    }
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

void ServerMainWindow::on_CloseButton_clicked()
{
    TcpServerSingleton::get_instance()->close_server();
    close();
}

void ServerMainWindow::on_SmallButton_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

