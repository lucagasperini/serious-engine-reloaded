#ifndef SSMF_MAINWINDOW_H
#define SSMF_MAINWINDOW_H

#include <Engine/Base/CTString.h>
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */
#include <Engine/Graphics/ViewPort.h> /* CViewPort */

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

    CDrawPort* dp;
    CViewPort* vp;

public:
    SEMainWindow();
    ~SEMainWindow();

    inline void setTitle(const CTString& _title) { title = _title; }
    inline void setX(PIX _x) { x = _x; }
    inline void setY(PIX _y) { y = _y; }
    inline void setW(PIX _w) { w = _w; }
    inline void setH(PIX _h) { h = _h; }
    inline void setAPI(GfxAPIType _api) { api = _api; }
    inline void setMode(INDEX _mode) { mode = _mode; }
    inline void setResizable(BOOL _resizable) { resizable = _resizable; }
    inline void setStatus(INDEX _status) { status = _status; }
    inline void setAdapter(INDEX _adapter) { adapter = _adapter; }
    inline void setDepth(DisplayDepth _depth) { depth = _depth; }
    inline void setDrawPort(const CDrawPort& _dp) { dp = new CDrawPort(_dp); }
    inline void setViewPort(const CViewPort& _vp) { vp = new CViewPort(_vp); }

    inline CTString getTitle() const { return title; }
    inline PIX getX() const { return x; }
    inline PIX getY() const { return y; }
    inline PIX getW() const { return w; }
    inline PIX getH() const { return h; }
    inline INDEX getAPI() const { return api; }
    inline INDEX getMode() const { return mode; }
    inline INDEX getResizable() const { return resizable; }
    inline INDEX getStatus() const { return status; }
    inline INDEX getAdapter() const { return adapter; }
    inline DisplayDepth getDepth() const { return depth; }
    inline CDrawPort* getDrawPort() const { return dp; }
    inline CViewPort* getViewPort() const { return vp; }

    BOOL isIconic();
    BOOL create();
    void destroy();
};

#endif