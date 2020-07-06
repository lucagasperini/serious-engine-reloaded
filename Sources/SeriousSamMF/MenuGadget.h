#ifndef SSMF_MENUGADGET_H
#define SSMF_MENUGADGET_H

#include <Engine/Base/CTString.h>
#include <Engine/Base/Types.h>
#include <Engine/Math/AABBox.h>
#include <Engine/Base/SDL/SDLEvents.h>

enum ButtonFontSize {
  BFS_SMALL = 0,
  BFS_MEDIUM = 1,
  BFS_LARGE  = 2,
};


class CMenuGadget {
public:
  CListNode mg_lnNode;
  FLOATaabbox2D mg_boxOnScreen;
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
  virtual void  Render( CDrawPort *pdp);
  virtual BOOL  IsSeparator(void) { return FALSE; };
};

class CMGButton : public CMenuGadget {
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
  CMGButton(void);
  void SetText( CTString strNew);
  void OnActivate(void);
  void Render( CDrawPort *pdp);
  PIX  GetCharOffset( CDrawPort *pdp, INDEX iCharNo);
};

#endif