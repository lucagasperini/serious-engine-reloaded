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

    inline void setVirtX(PIX _virtX) { virtX = _virtX; }
    inline void setVirtY(PIX _virtY) { virtY = _virtY; }
    inline void setDrawPort(const CDrawPort &_drawPort) { drawPort = new CDrawPort(_drawPort); }
    inline void setViewPort(const CViewPort &_viewPort) { viewPort = new CViewPort(_viewPort); }

    inline PIX getVirtX() const { return virtX; }
    inline PIX getVirtY() const { return virtY; }
    inline const CDrawPort* getDrawPort() const { return drawPort; } 
    inline const CViewPort* getViewPort() const { return viewPort; } 

    void destroy();
    BOOL create(HWND _window);

    void fill(COLOR col) const;
    void fillZBuffer(FLOAT zval) const;
    void SwapBuffers();

    PIX width();
    PIX height();

    void putTexture(class CTextureObject *pTO, PIX _x, PIX _y, PIX _w, PIX _h) const;
    CTextureObject* loadTexture(const CTFileName &fnm) const;

    void setText(CFontData *font, INDEX textmode = 1) const;

    PIXaabbox2D box2D(FLOAT x, FLOAT y, FLOAT w, FLOAT h) const;

    BOOL lock();
    void unlock();
};

#endif