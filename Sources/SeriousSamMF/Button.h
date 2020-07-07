#ifndef SSMF_BUTTON_H
#define SSMF_BUTTON_H

#include "MenuGadget.h"
#include "Render.h"
#include "Object.h"


class SEButton : public CMenuGadget {
public:
  CTString mg_strLabel;   // for those that have labels separately from main text
  CTString mg_strText;
  INDEX mg_iCenterI;
  ButtonFontSize mg_bfsFontSize;
  BOOL  mg_bEditing;
  BOOL  mg_bHighlighted;
  BOOL  mg_bRectangle;
  BOOL  mg_bMental;
  INDEX mg_iTextMode;
  INDEX mg_iCursorPos;

  INDEX mg_iIndex;
  void (*mg_pActivatedFunction)(void);

  COLOR colEnable;
  COLOR colDisable;
  COLOR colSelected;
public:
  SEButton();
  ~SEButton();

  void SetText( CTString strNew);
  void OnActivate();
  void render(const SERender *render);
  PIX  GetCharOffset( CDrawPort *pdp, INDEX iCharNo);

  COLOR currentColor();
};

#endif