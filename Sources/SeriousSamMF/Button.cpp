#include "Button.h"

#include <Engine/Graphics/DrawPort.h>

#include "Colors.h"
#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Font.h>

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
{/*
  if( mg_pActivatedFunction!=NULL && mg_bEnabled)
  {
    PlayMenuSound(_psdPress);
    IFeel_PlayEffect("Menu_press");
    _pmgLastActivatedGadget = this;
    (*mg_pActivatedFunction)();
  }*/
}


COLOR LCDGetColor(COLOR colDefault, const char *strName)
{
  if (!strcmp(strName, "thumbnail border")) {
    colDefault = SE_COL_BLUE_NEUTRAL|255;
  } else if (!strcmp(strName, "no thumbnail")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "popup box")) {
    colDefault = SE_COL_BLUE_NEUTRAL|255;
  } else if (!strcmp(strName, "tool tip")) {
    colDefault = SE_COL_ORANGE_LIGHT|255;
  } else if (!strcmp(strName, "unselected")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "selected")) {
    colDefault = SE_COL_ORANGE_LIGHT|255;
  } else if (!strcmp(strName, "disabled selected")) {
    colDefault = SE_COL_ORANGE_DARK_LT |255;
  } else if (!strcmp(strName, "disabled unselected")) {
    colDefault = SE_COL_ORANGE_DARK|255;
  } else if (!strcmp(strName, "label")) {
    colDefault = C_WHITE|255;
  } else if (!strcmp(strName, "title")) {
    colDefault = C_WHITE|255;
  } else if (!strcmp(strName, "editing")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "hilited")) {
    colDefault = SE_COL_ORANGE_LIGHT|255;
  } else if (!strcmp(strName, "hilited rectangle")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "edit fill")) {
    colDefault = SE_COL_BLUE_DARK_LT|75;
  } else if (!strcmp(strName, "editing cursor")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "model box")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "hiscore header")) {
    colDefault = SE_COL_ORANGE_LIGHT|255;
  } else if (!strcmp(strName, "hiscore data")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "hiscore last set")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "slider box")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "file info")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "display mode")) {
    colDefault = SE_COL_ORANGE_NEUTRAL|255;
  } else if (!strcmp(strName, "bcg fill")) {
    colDefault = SE_COL_BLUE_DARK|255;
  }
  return colDefault;
}


extern CFontData _fdBig;
void SetFontBig(CDrawPort *pdp)
{
  pdp->SetFont( &_fdBig);
  pdp->SetTextScaling( 1.0f * pdp->GetWidth() /640 *pdp->dp_fWideAdjustment);
  pdp->SetTextAspect(1.0f);
}
extern CFontData _fdMedium;
void SetFontMedium(CDrawPort *pdp)
{
  pdp->SetFont( &_fdMedium);
  pdp->SetTextScaling( 1.0f * pdp->GetWidth() /640 *pdp->dp_fWideAdjustment);
  pdp->SetTextAspect(0.75f);
}/*
void SetFontSmall(CDrawPort *pdp)
{
  pdp->SetFont( _pfdConsoleFont);
  pdp->SetTextScaling( 1.0f);
  pdp->SetTextAspect(1.0f);
}
*/

 
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
    return colEnable;
}

void SEButton::render(const SERender* render)
{
  if (mg_bfsFontSize==BFS_LARGE) {
    render->setText(&_fdBig, mg_iTextMode);
  } else if (mg_bfsFontSize==BFS_MEDIUM) {
    render->setText(&_fdMedium, mg_iTextMode);
  } else {/*
    ASSERT(mg_bfsFontSize==BFS_SMALL);
    SetFontSmall(pdp);*/
  }

  const CDrawPort* pdp = render->getDrawPort();

  COLOR col = currentColor();
  if(mg_bEditing) {
    col = LCDGetColor(C_GREEN|0xFF, "editing");
  }
  
  COLOR colRectangle = col;
  if( mg_bHighlighted) {
    col = LCDGetColor(C_WHITE|0xFF, "hilited");
    if( !mg_bFocused) {
      colRectangle = LCDGetColor(C_WHITE|0xFF, "hilited rectangle");
    }
  }/*
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
         if( mg_iCenterI==-1) pdp->PutText(  str, mg_boxOnScreen.Min()(1),    mg_boxOnScreen.Min()(2), col);
    else if( mg_iCenterI==+1) pdp->PutTextR( str, mg_boxOnScreen.Max()(1),    mg_boxOnScreen.Min()(2), col);
    else                      pdp->PutTextC( str, mg_boxOnScreen.Center()(1), mg_boxOnScreen.Min()(2), col);
  }
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