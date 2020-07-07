#include "Render.h"
#include "Colors.h"

#include <Engine/Engine.h>


SERender::SERender()
{
    virtX = 1280;
    virtY = 720;
}
SERender::~SERender()
{

}

void SERender::destroy()
{
    if( viewPort != NULL) {
        _pGfx->DestroyWindowCanvas(viewPort);
        viewPort = NULL;
    }
}

BOOL SERender::create(HWND _window)
{
    _pGfx->CreateWindowCanvas( _window, &viewPort, &drawPort);
    
    // initial screen fill and swap, just to get context running
    if( drawPort!=NULL && drawPort->Lock()) {
      drawPort->Fill(SE_COL_ORANGE_NEUTRAL|255);
      drawPort->Unlock();
      viewPort->SwapBuffers();
    }

    return TRUE;
}


void SERender::fill(COLOR col) const
{
    drawPort->Fill(col);
}

void SERender::fillZBuffer(FLOAT zval) const
{
    drawPort->FillZBuffer(zval);
}

BOOL SERender::lock()
{
    return drawPort->Lock();
}

void SERender::unlock()
{
    drawPort->Unlock();
}

void SERender::SwapBuffers()
{
    viewPort->SwapBuffers();
}


PIX SERender::width()
{
    return drawPort->GetWidth();
}
PIX SERender::height()
{
    return drawPort->GetHeight();
}

void SERender::putTexture(class CTextureObject *texture, PIX _x, PIX _y, PIX _w, PIX _h) const
{
    FLOAT scaleX = (FLOAT)drawPort->GetWidth() / (FLOAT)virtX;
    FLOAT scaleY = (FLOAT)drawPort->GetHeight() / (FLOAT)virtY;
    PIX   pixX, pixY, pixW, pixH;

    pixX = _x * scaleX;
    pixY = _y * scaleY;
    pixW = pixX + ( _w * scaleX );
    pixH = pixY + ( _h * scaleY );

    drawPort->PutTexture( texture, PIXaabbox2D( PIX2D(pixX, pixY), PIX2D(pixW, pixH)));
}

CTextureObject* SERender::loadTexture(const CTFileName &fntex) const
{
    CTextureObject* tex = new CTextureObject;
    tex->SetData_t(fntex);
    return tex;
}

void SERender::setText(CFontData *font, INDEX textmode) const
{
    FLOAT scaleY = (FLOAT)drawPort->GetHeight() / (FLOAT)virtY;

    drawPort->SetFont(font);
    drawPort->SetTextScaling(scaleY);
    drawPort->SetTextAspect(1.0f);
    drawPort->SetTextMode(textmode);
    //drawPort->SetTextCharSpacing();
    //drawPort->SetTextLineSpacing();
}

PIXaabbox2D SERender::box2D(FLOAT x, FLOAT y, FLOAT w, FLOAT h) const
{
    PIX pixW = drawPort->GetWidth();
    PIX pixH = drawPort->GetHeight();

    return PIXaabbox2D(PIX2D( x * pixW, y * pixH ), PIX2D( w * pixW, h * pixH));
}