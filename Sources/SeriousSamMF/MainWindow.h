#ifndef SSMF_MAINWINDOW_H
#define SSMF_MAINWINDOW_H

#include <Engine/Base/CTString.h>
#include <Engine/Graphics/ViewPort.h> /* CViewPort */
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */

class SEMainWindow {
private:
    CTString title;
    HWND pWindow;
    PIX x, y, w, h;

    GfxAPIType api;
    INDEX mode;
    BOOL resizable;
    /* FIXME: find a better name! */
    INDEX status;
    /* FIXME: what is adapter? it's called on SetDisplayMode() function on create method */
    INDEX adapter;

    DisplayDepth depth;
    CDrawPort* drawPort;
    CViewPort* viewPort;
public:
    SEMainWindow();
    ~SEMainWindow();

    void setTitle(const CTString &_title) { title = _title; }
    void setPWindow(const HWND _pWindow) { pWindow = _pWindow; }
    void setX(PIX _x) { x = _x; }
    void setY(PIX _y) { y = _y; }
    void setW(PIX _w) { w = _w; }
    void setH(PIX _h) { h = _h; }
    void setAPI(GfxAPIType _api) { api = _api; }
    void setMode(INDEX _mode) { mode = _mode; }
    void setResizable(BOOL _resizable) { resizable = _resizable; }
    void setStatus(INDEX _status) { status = _status; }
    void setAdapter(INDEX _adapter) { adapter = _adapter; }
    void setDepth(DisplayDepth _depth) { depth = _depth; }
    void setDrawPort(const CDrawPort &_drawPort) { drawPort = new CDrawPort(_drawPort); }
    void setViewPort(const CViewPort &_viewPort) { viewPort = new CViewPort(_viewPort); }

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
    INDEX getAdapter() { return adapter; }
    DisplayDepth getDepth() { return depth; }
    CDrawPort* getDrawPort() { return drawPort; }
    CViewPort* getViewPort() { return viewPort; }

    BOOL isIconic();
    BOOL create();
    void destroy();
};

#endif