#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    engine1 = new SEThread(ui->widget);
    engine1->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
