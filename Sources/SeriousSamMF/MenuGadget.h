#ifndef SSMF_MENUGADGET_H
#define SSMF_MENUGADGET_H

#include <Engine/Base/CTString.h>
#include <Engine/Base/Types.h>
#include <Engine/Math/AABBox.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include "Render.h"

enum ButtonFontSize {
  BFS_SMALL = 0,
  BFS_MEDIUM = 1,
  BFS_LARGE  = 2,
};


class CMenuGadget {
public:
  CListNode mg_lnNode;
  PIXaabbox2D mg_boxOnScreen;
  BOOL mg_bVisible;
  BOOL mg_bEnabled;
  BOOL mg_bLabel;
  BOOL mg_bFocused;
  INDEX mg_iInList; // for scrollable gadget lists
  
  CTString mg_strTip;
  CMenuGadget *mg_pmgLeft;
  CMenuGadget *mg_pmgRight;
  CMenuGadget *mg_pmgUp;
  CMenuGadget *mg_pmgDown;

  CMenuGadget( void);
  // return TRUE if handled
  virtual BOOL OnKeyDown( int iVKey);
  virtual BOOL OnChar(MSG msg);
  virtual void OnActivate( void);
  virtual void OnSetFocus( void);
  virtual void OnKillFocus( void);
  virtual void Appear( void);
  virtual void Disappear( void);
  virtual void Think( void);
  virtual void OnMouseOver(PIX pixI, PIX pixJ);

  virtual COLOR GetCurrentColor(void);
  virtual void  Render( SERender *render);
  virtual BOOL  IsSeparator(void) { return FALSE; };
};

#endif