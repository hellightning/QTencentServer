#include "servermainwindow.h"
#include "ui_servermainwindow.h"

ServerMainWindow::ServerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerMainWindow)
{
    ui->setupUi(this);
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}
