#include "MainWindow.h"
#include <QApplication>

#include <Engine/Engine.h>

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
char* argv0 = NULL;

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    argv0 = argv[0];

    CTString cmdline;
    for (int i = 1; i < argc; i++) {
        cmdline += " \"";
        cmdline += argv[i];
        cmdline += "\"";
    }
    MainWindow w;

    w.show();
    return a.exec();
}
