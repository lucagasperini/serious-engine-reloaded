#include "Render.h"
#include "Colors.h"

#include <Engine/Engine.h>
#include <Engine/Graphics/ImageInfo.h>


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

void SERender::putTexture(class CTextureObject *texObj, PIX _x, PIX _y, PIX _w, PIX _h) const
{
    FLOAT scaleX = (FLOAT)drawPort->GetWidth() / (FLOAT)virtX;
    FLOAT scaleY = (FLOAT)drawPort->GetHeight() / (FLOAT)virtY;
    PIX   pixX, pixY, pixW, pixH;

    pixX = _x * scaleX;
    pixY = _y * scaleY;
    pixW = pixX + ( _w * scaleX );
    pixH = pixY + ( _h * scaleY );

    drawPort->PutTexture( texObj, PIXaabbox2D( PIX2D(pixX, pixY), PIX2D(pixW, pixH)));
}

void SERender::putTexture(class CTextureObject *texObj, PIX _x, PIX _y) const
{
    CTextureData* texData = (CTextureData*) texObj->GetData();
    PIX _w = texData->GetPixWidth();
    PIX _h = texData->GetPixHeight();
    
    FLOAT scaleX = (FLOAT)drawPort->GetWidth() / (FLOAT)virtX;
    FLOAT scaleY = (FLOAT)drawPort->GetHeight() / (FLOAT)virtY;
    PIX   pixX, pixY, pixW, pixH;

    pixX = _x * scaleX;
    pixY = _y * scaleY;
    pixW = pixX + ( _w * scaleX );
    pixH = pixY + ( _h * scaleY );

    drawPort->PutTexture( texObj, PIXaabbox2D( PIX2D(pixX, pixY), PIX2D(pixW, pixH)));
}

BOOL SERender::tga2tex(const CTFileName &fntex) const
{/*
    CTextureData* data = new CTextureData();
    CImageInfo* image = new CImageInfo();
    image->LoadAnyGfxFormat_t(fntex);
    data->Create_t(image,1024,1,FALSE);
    CTextureObject* tex = new CTextureObject;
    tex->SetData((CAnimData*)data);
*/
    CImageInfo iiImageInfo;
   iiImageInfo.LoadAnyGfxFormat_t( fntex);
    // both dimension must be potentions of 2
    if( (iiImageInfo.ii_Width  == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Width))) &&
        (iiImageInfo.ii_Height == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Height))) )
    {
      CTFileName fnTexture = fntex.FileDir()+fntex.FileName()+".tex";
      // creates new texture with one frame
      CTextureData tdPicture;
      tdPicture.Create_t( &iiImageInfo, iiImageInfo.ii_Width, 1, FALSE);
      tdPicture.Save_t( fnTexture);
    }
    return TRUE;
}

CTextureObject* SERender::loadTexture(const CTFileName &fntex) const
{
    CTextureObject* tex = new CTextureObject;
    tex->SetData_t(fntex);
    return tex;
}

void SERender::setText(CFontData *font, FLOAT addScale, INDEX textmode) const
{
    FLOAT scaleY = (FLOAT)drawPort->GetHeight() / (FLOAT)virtY;

    drawPort->SetFont(font);
    drawPort->SetTextScaling(scaleY + addScale);
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