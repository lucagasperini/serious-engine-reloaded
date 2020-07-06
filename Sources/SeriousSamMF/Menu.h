#ifndef SSMF_MENU_H
#define SSMF_MENU_H

#include <Engine/Graphics/Texture.h>
#include "Render.h"

class SEMenu 
{
private:
    CTextureObject _toPointer;
    CTextureObject _toLogoMenuA;
    CTextureObject _toLogoMenuB;

    SERender* render;
public:
// logo textures
    CTextureObject  _toLogoCT;
    CTextureObject  _toLogoODI;
    CTextureObject  _toLogoEAX;
    CTextureObject *_ptoLogoCT  = NULL;
    CTextureObject *_ptoLogoODI = NULL;
    CTextureObject *_ptoLogoEAX = NULL;
    BOOL active;
    SEMenu();
    ~SEMenu();

    void setActive(BOOL _active) { active = _active; }

    BOOL getActive() { return active; }

    void LoadAndForceTexture(CTextureObject &to, CTextureObject *&pto, const CTFileName &fnm);

    // interface rendering functions
  void LCDInit(void);
  void LCDEnd(void);
  void LCDPrepare(FLOAT fFade);
  void LCDSetDrawport(CDrawPort *pdp);
  void LCDDrawBox(PIX pixUL, PIX pixDR, const PIXaabbox2D &box, COLOR col);
  void LCDScreenBox(COLOR col);
  void LCDScreenBoxOpenLeft(COLOR col);
  void LCDScreenBoxOpenRight(COLOR col);
  void LCDRenderClouds1(void);
  void LCDRenderClouds2(void);
    void LCDRenderCloudsForComp(void);
    void LCDRenderCompGrid(void);
    void LCDRenderGrid(void);
    void LCDDrawPointer(PIX pixI, PIX pixJ);
    COLOR LCDGetColor(COLOR colDefault, const char *strName);
    COLOR LCDFadedColor(COLOR col);
    COLOR LCDBlinkingColor(COLOR col0, COLOR col1);


    void init(SERender* _render);
    BOOL run();
};

#endif