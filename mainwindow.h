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

    virtual void paintEvent(QPaintEvent*);

protected:
    void clean_backlog_list();

private slots:
    void on_closeServerButton_clicked();
    void on_openServerButton_clicked();
    void on_online_increase(int);
    void on_online_decrease(int);

    void add_backlog_list(QString);//在info中的listWidget里显示消息
    void slot_get_ip_list(QHostInfo);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
