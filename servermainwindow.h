#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ServerMainWindow;
}

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();

private:
    Ui::ServerMainWindow *ui;
};

#endif // SERVERMAINWINDOW_H
