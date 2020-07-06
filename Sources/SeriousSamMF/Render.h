#ifndef SSMF_RENDER_H
#define SSMF_RENDER_H

#include <Engine/Graphics/ViewPort.h> /* CViewPort */
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */

class SERender {
private:
    PIX virtX, virtY;
    CDrawPort* drawPort;
    CViewPort* viewPort;
public:
    SERender();
    ~SERender();

    void setVirtX(PIX _virtX) { virtX = _virtX; }
    void setVirtY(PIX _virtY) { virtY = _virtY; }
    void setDrawPort(const CDrawPort &_drawPort) { drawPort = new CDrawPort(_drawPort); }
    void setViewPort(const CViewPort &_viewPort) { viewPort = new CViewPort(_viewPort); }

    PIX getVirtX() { return virtX; }
    PIX getVirtY() { return virtY; }
    CDrawPort* getDrawPort() { return drawPort; }
    CViewPort* getViewPort() { return viewPort; }

    void destroy();
    BOOL create(HWND _window);

    void fill(COLOR col);
    void fillZBuffer(FLOAT zval);
    void SwapBuffers();

    PIX width();
    PIX height();

    void putTexture(class CTextureObject *pTO, PIX _x, PIX _y, PIX _w, PIX _h);
    CTextureObject* loadTexture(const CTFileName &fnm);

    BOOL lock();
    void unlock();
};

#endif