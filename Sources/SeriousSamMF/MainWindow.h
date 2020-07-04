#ifndef SSMF_MAINWINDOW_H
#define SSMF_MAINWINDOW_H

#include <Engine/Base/CTString.h>

class SEMainWindow {
private:
    CTString str_title;
    HWND hwndMain;
    // for window reposition function
    PIX _pixLastSizeI, _pixLastSizeJ;
public:
    SEMainWindow();
    ~SEMainWindow();

    void setTitle(const CTString &_str_title) { str_title = _str_title; }
    void setHandler(const HWND _hwndMain) { hwndMain = _hwndMain; }

    CTString getTitle() { return str_title; }
    HWND getHandler() { return hwndMain; }

    void close();
    void open(BOOL _fullscreen, PIX pixSizeI, PIX pixSizeJ);
};

#endif