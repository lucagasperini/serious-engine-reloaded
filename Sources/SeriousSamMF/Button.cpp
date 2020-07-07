#include "Button.h"

#include <Engine/Graphics/DrawPort.h>

#include "Colors.h"
#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Graphics/Texture.h>

SEButton::SEButton()
{
  mg_pActivatedFunction = NULL;
  mg_iIndex = 0;
  mg_iCenterI = 0;
  mg_iTextMode = 1;
  mg_bfsFontSize = BFS_MEDIUM;
  mg_iCursorPos = -1;
  mg_bRectangle = FALSE;
  mg_bMental = FALSE;
  colEnable = C_BLACK|255;
  colSelected = C_BLACK|255;
  colDisable = C_BLACK|255;
}

SEButton::~SEButton()
{

}


void SEButton::SetText( CTString strNew)
{
  mg_strText = strNew;
}


void SEButton::OnActivate()
{
    printf("Active");
    if( mg_pActivatedFunction!=NULL )
    {/*
      PlayMenuSound(_psdPress);
      IFeel_PlayEffect("Menu_press");
      _pmgLastActivatedGadget = this;*/
      (*mg_pActivatedFunction)();
    }
}


PIXaabbox2D FloatBoxToPixBox(const CDrawPort *pdp, const FLOATaabbox2D &boxF)
{
  PIX pixW = pdp->GetWidth();
  PIX pixH = pdp->GetHeight();
  return PIXaabbox2D(
    PIX2D(boxF.Min()(1)*pixW, boxF.Min()(2)*pixH),
    PIX2D(boxF.Max()(1)*pixW, boxF.Max()(2)*pixH));
}

FLOATaabbox2D PixBoxToFloatBox(const CDrawPort *pdp, const PIXaabbox2D &boxP)
{
  FLOAT fpixW = pdp->GetWidth();
  FLOAT fpixH = pdp->GetHeight();
  return FLOATaabbox2D(
    FLOAT2D(boxP.Min()(1)/fpixW, boxP.Min()(2)/fpixH),
    FLOAT2D(boxP.Max()(1)/fpixW, boxP.Max()(2)/fpixH));
}

COLOR SEButton::currentColor()
{
    if(mg_bFocused)
        return colSelected;
    else
        return colEnable;
}

extern CFontData _fdBig;
extern CFontData _fdMedium;
void SEButton::render(const SERender* render)
{
  if (mg_bfsFontSize==BFS_LARGE) {
    render->setText(&_fdBig, 0.5f, mg_iTextMode);
  } else if (mg_bfsFontSize==BFS_MEDIUM) {
    render->setText(&_fdMedium, 0.0f, mg_iTextMode);
  } else {/*
    ASSERT(mg_bfsFontSize==BFS_SMALL);
    SetFontSmall(pdp);*/
  }

  const CDrawPort* pdp = render->getDrawPort();

  COLOR col = currentColor();/*
  if(mg_bEditing) {
    col = LCDGetColor(C_GREEN|0xFF, "editing");
  }
  
  COLOR colRectangle = col;
  if( mg_bHighlighted) {
    col = LCDGetColor(C_WHITE|0xFF, "hilited");
    if( !mg_bFocused) {
      colRectangle = LCDGetColor(C_WHITE|0xFF, "hilited rectangle");
    }
  }
  */
  /*
  if (mg_bMental) {
    FLOAT tmIn   = 0.2f;
    FLOAT tmOut  = 1.0f;
    FLOAT tmFade = 0.1f;
    FLOAT tmExist = tmFade+tmIn+tmFade;
    FLOAT tmTotal = tmFade+tmIn+tmFade+tmOut;
    
    FLOAT tmTime = _pTimer->GetHighPrecisionTimer().GetSeconds();
    FLOAT fFactor = 1;
    if (tmTime>0.1f) {
      tmTime = fmod(tmTime, tmTotal);
      fFactor = CalculateRatio(tmTime, 0, tmExist, tmFade/tmExist, tmFade/tmExist);
    }
    col = (col&~0xFF)|INDEX(0xFF*fFactor);
  }*/
/*
  if( mg_bRectangle) {
    // put border
    const PIX pixLeft   = mg_boxOnScreen.Min()(1);
    const PIX pixUp     = mg_boxOnScreen.Min()(2)-3;
    const PIX pixWidth  = mg_boxOnScreen.Size()(1)+1;
    const PIX pixHeight = mg_boxOnScreen.Size()(2);
    pdp->DrawBorder( pixLeft, pixUp, pixWidth, pixHeight, colRectangle);
  }

  if( mg_bEditing) {
    // put border
    PIX pixLeft   = mg_boxOnScreen.Min()(1);
    PIX pixUp     = mg_boxOnScreen.Min()(2)-3;
    PIX pixWidth  = mg_boxOnScreen.Size()(1)+1;
    PIX pixHeight = mg_boxOnScreen.Size()(2);
    if (mg_strLabel!="") {
      pixLeft = (PIX) (mg_boxOnScreen.Min()(1)+mg_boxOnScreen.Size()(1)*0.55f);
      pixWidth = (PIX) (mg_boxOnScreen.Size()(1)*0.45f+1);
    }
    pdp->Fill( pixLeft, pixUp, pixWidth, pixHeight, LCDGetColor(C_dGREEN|0x40, "edit fill"));
  }
*/

  if( mg_iCenterI==-1) 
      pdp->PutText(mg_strText, mg_boxOnScreen.Min()(1),mg_boxOnScreen.Min()(2), col);
  else if( mg_iCenterI==+1) 
      pdp->PutTextR( mg_strText, mg_boxOnScreen.Max()(1),    mg_boxOnScreen.Min()(2), col);
  else  
      pdp->PutTextC( mg_strText, mg_boxOnScreen.Center()(1), mg_boxOnScreen.Min()(2), col);
/*
  INDEX iCursor = mg_iCursorPos;

  // print text
  if (mg_strLabel!="") {
    PIX pixIL = (PIX) (mg_boxOnScreen.Min()(1)+mg_boxOnScreen.Size()(1)*0.45f);
    PIX pixIR = (PIX) (mg_boxOnScreen.Min()(1)+mg_boxOnScreen.Size()(1)*0.55f);
    PIX pixJ  = (PIX) (mg_boxOnScreen.Min()(2));

    pdp->PutTextR( mg_strLabel, pixIL, pixJ, col);
    pdp->PutText( mg_strText, pixIR, pixJ, col);
  } else {
    CTString str = mg_strText;
    if (pdp->dp_FontData->fd_bFixedWidth) {
      str = str.Undecorated();
      //INDEX iLen = str.Length();
      INDEX iMaxLen = ClampDn(mg_boxOnScreen.Size()(1)/(pdp->dp_pixTextCharSpacing+pdp->dp_FontData->fd_pixCharWidth), 1);
      if (iCursor>=iMaxLen) {
        str.TrimRight(iCursor);
        str.TrimLeft(iMaxLen);
        iCursor = iMaxLen;
      } else {
        str.TrimRight(iMaxLen);
      }
    }
   
  }
  */
/*
  // put cursor if editing
  if( mg_bEditing && (((ULONG)(_pTimer->GetRealTimeTick()*2))&1)) {
    PIX pixX = box.Min()(1) + GetCharOffset( pdp, iCursor);
    if (mg_strLabel!="") {
      pixX += (PIX) (box.Size()(1)*0.55f);
    }
    PIX pixY = (PIX) (box.Min()(2));
    if (!pdp->dp_FontData->fd_bFixedWidth) {
      pixY -= (PIX) (pdp->dp_fTextScaling *2);
    }
    pdp->PutText( "|", pixX, pixY, LCDGetColor(C_WHITE|0xFF, "editing cursor"));
  }
  */
}


PIX SEButton::GetCharOffset( CDrawPort *pdp, INDEX iCharNo)
{/*
  if (pdp->dp_FontData->fd_bFixedWidth) {
    return ((PIX) ((pdp->dp_FontData->fd_pixCharWidth+pdp->dp_pixTextCharSpacing)*(iCharNo-0.5f)));
  }
  CTString strCut(mg_strText);
  strCut.TrimLeft( strlen(mg_strText)-iCharNo);
  PIX pixFullWidth = pdp->GetTextWidth(mg_strText);
  PIX pixCutWidth  = pdp->GetTextWidth(strCut);
  // !!!! not implemented for different centering
  return pixFullWidth-pixCutWidth;*/
}