#ifndef SETHREAD_H
#define SETHREAD_H

#include <QObject>
#include <QThread>
#include <QWidget>

class SEThread : public QThread
{
public:
    SEThread(QWidget* _win);
    void run();
};

#endif // SETHREAD_H
