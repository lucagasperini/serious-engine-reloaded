#ifndef SSMF_MAINWINDOW_H
#define SSMF_MAINWINDOW_H

#include <Engine/Base/CTString.h>

class SEMainWindow {
private:
    CTString title;
    HWND pWindow;
    PIX x, y, w, h;

    INDEX api;
    INDEX mode;
    BOOL resizable;
    /* FIXME: find a better name! */
    INDEX status;
public:
    SEMainWindow();
    ~SEMainWindow();

    void setTitle(const CTString &_title) { title = _title; }
    void setPWindow(const HWND _pWindow) { pWindow = _pWindow; }
    void setX(PIX _x) { x = _x; }
    void setY(PIX _y) { y = _y; }
    void setW(PIX _w) { w = _w; }
    void setH(PIX _h) { h = _h; }
    void setAPI(INDEX _api) { api = _api; }
    void setMode(INDEX _mode) { mode = _mode; }
    void setResizable(BOOL _resizable) { resizable = _resizable; }
    void setStatus(INDEX _status) { status = _status; }

    CTString getTitle() { return title; }
    HWND getPWindow() { return pWindow; }
    PIX getX() { return x; }
    PIX getY() { return y; }
    PIX getW() { return w; }
    PIX getH() { return h; }
    INDEX getAPI() { return api; }
    INDEX getMode() { return mode; }
    INDEX getResizable() { return resizable; }
    INDEX getStatus() { return status; }

    void close();
    void open();
};

#endif