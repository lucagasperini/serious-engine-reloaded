#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Thread.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    SEThread* engine1;
    SEThread* engine2;
    SEThread* engine3;
    SEThread* engine4;
};

#endif // MAINWINDOW_H
