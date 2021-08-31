#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpserversingleton.h"

typedef int QtId;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();
    void add_backlog_list();
    void clean_backlog_list();

    virtual void paintEvent(QPaintEvent*);


private slots:
    void on_closeServerButton_clicked();

    void on_openServerButton_clicked();

    void on_online_increase(int);

    void on_online_decrease(int);

    void slot_get_ip_list(QHostInfo);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
