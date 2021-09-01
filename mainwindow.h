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
    /**
     * @brief ServerMainWindow 构造函数,初始化ui
     * @param parent 父亲对象的指针
     */
    ServerMainWindow(QWidget *parent = nullptr);
    /**
     * @brief ~ServerMainWindow 析构函数
     */
    ~ServerMainWindow();


protected:
    /**
     * @brief clean_backlog_list 当Info框内的记录大于1000条时删去500条
     */
    void clean_backlog_list();
    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param e
     */
    void mousePressEvent(QMouseEvent* e);
    /**
     * @brief mouseReleaseEvent 鼠标释放事件
     * @param e
     */
    void mouseReleaseEvent(QMouseEvent* e);
    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param e
     */
    void mouseMoveEvent(QMouseEvent* e);

private slots:
    /**
     * @brief on_closeServerButton_clicked 关闭服务器按钮按下时关闭服务器
     */
    void on_closeServerButton_clicked();
    /**
     * @brief on_openServerButton_clicked 打开服务器按钮按下时打开服务器
     */
    void on_openServerButton_clicked();
    /**
     * @brief on_online_increase 用户上线,加入到ListWidget中
     */
    void on_online_increase(int);
    /**
     * @brief on_online_decrease 用户下线,从ListWidget中删去
     */
    void on_online_decrease(int);

    /**
     * @brief add_backlog_list 在info中的listWidget里显示消息
     */
    void add_backlog_list(QString);
    /**
     * @brief slot_get_ip_list 更新combobox里的ip
     */
    void slot_get_ip_list(QHostInfo);

    /**
     * @brief on_CloseButton_clicked 关闭窗口按钮按下时关闭服务器和ui并结束程序
     */
    void on_CloseButton_clicked();

    /**
     * @brief on_SmallButton_clicked 最小化窗口按钮按下时最小化窗口
     */
    void on_SmallButton_clicked();

private:

    void drawOutLine(QPainter &painter);

private:
    Ui::MainWindow *ui;

    bool isDrag = false;
    QPoint mouse_start_point;
    QPoint window_topleft_point;
};
#endif // MAINWINDOW_H
