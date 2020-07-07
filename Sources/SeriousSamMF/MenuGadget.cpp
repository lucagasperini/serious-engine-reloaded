#include "MenuGadget.h"


CMenuGadget::CMenuGadget( void)
{
  mg_pmgLeft = NULL;
  mg_pmgRight = NULL;
  mg_pmgUp = NULL;
  mg_pmgDown = NULL;

  mg_bVisible = TRUE;
  mg_bEnabled = TRUE;
  mg_bLabel = FALSE;
  mg_bFocused = FALSE;
  mg_iInList = -1;    // not in list
}

void CMenuGadget::OnActivate( void)
{
  NOTHING;
}

// return TRUE if handled
BOOL CMenuGadget::OnKeyDown( int iVKey)
{
  // if return pressed
  if( iVKey==VK_RETURN || iVKey==VK_LBUTTON) {
    // activate
    OnActivate();
    // key is handled
    return TRUE;
  }
  // key is not handled
  return FALSE;
}


BOOL CMenuGadget::OnChar(MSG msg)
{
  // key is not handled
  return FALSE;
}


void CMenuGadget::OnSetFocus( void)
{/*
  mg_bFocused = TRUE;
  if( !IsSeparator())
  {
    PlayMenuSound(_psdSelect);
    IFeel_PlayEffect("Menu_select");
  }*/
}

void CMenuGadget::OnKillFocus( void)
{
  mg_bFocused = FALSE;
}

void CMenuGadget::Appear( void)
{
  mg_bVisible = TRUE;
}

void CMenuGadget::Disappear( void)
{
  mg_bVisible = FALSE;
  mg_bFocused = FALSE;
}

void CMenuGadget::Think( void)
{
}
void CMenuGadget::OnMouseOver(PIX pixI, PIX pixJ)
{
}

// get current color for the gadget
COLOR CMenuGadget::GetCurrentColor(void)
{/*
  // use normal colors
  COLOR colUnselected = _pGame->LCDGetColor(C_GREEN, "unselected");
  COLOR colSelected   = _pGame->LCDGetColor(C_WHITE, "selected");
  // if disabled
  if (!mg_bEnabled) {
    // use a bit darker colors
    colUnselected = _pGame->LCDGetColor(C_dGREEN, "disabled unselected");
    colSelected   = _pGame->LCDGetColor(C_GRAY, "disabled selected");
    // if label
    if (mg_bLabel) {
      // use white
      colUnselected = colSelected = _pGame->LCDGetColor(C_WHITE, "label");
    }
  }
  // use unselected color
  COLOR colRet = colUnselected;
  // if selected
  if( mg_bFocused) {
    // oscilate towards selected color
    FLOAT tmNow = _pTimer->GetHighPrecisionTimer().GetSeconds();
    colRet = LerpColor( (colUnselected>>1)&0x7F7F7F7F, colSelected, sin(tmNow*10.0f)*0.5f+0.5f);
  }

  return colRet|CT_OPAQUE;*/
}

void CMenuGadget::render(const SERender *render)
{
}

void CMenuGadget::update(POINT cursor)
{
      if(
        mg_boxOnScreen.Min()(1) < cursor.x && 
        mg_boxOnScreen.Min()(2) < cursor.y && 
        mg_boxOnScreen.Max()(1) > cursor.x &&
        mg_boxOnScreen.Max()(2) > cursor.y
    ){
       mg_bFocused = TRUE;
    } else {
      mg_bFocused = FALSE;
    }
}