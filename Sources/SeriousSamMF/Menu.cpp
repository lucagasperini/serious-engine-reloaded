#include "Menu.h"
#include <Engine/Base/ErrorReporting.h> /* FatalError() */
#include <Engine/Graphics/ViewPort.h> /* CViewPort */
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */
#include <Engine/Math/AABBox.h> /* PIXaabbox2D */

SEMenu::SEMenu()
{
    active = FALSE;
}

SEMenu::~SEMenu()
{
    
}

void SEMenu::LoadAndForceTexture(CTextureObject &to, CTextureObject *&pto, const CTFileName &fnm)
{
  try {
    to.SetData_t(fnm);
    CTextureData *ptd = (CTextureData*)to.GetData();
    ptd->Force( TEX_CONSTANT);
    ptd = ptd->td_ptdBaseTexture;
    if( ptd!=NULL) ptd->Force( TEX_CONSTANT);
    pto = &to;
  } catch( char *pchrError) {
    (void*)pchrError;
    pto = NULL;
  }
}

void SEMenu::init(SERender *_render)
{
    
  try {
    render = _render;
      // load logo textures
  LoadAndForceTexture(_toLogoCT,   _ptoLogoCT,   CTFILENAME("Textures\\Logo\\LogoCT.tex"));
  LoadAndForceTexture(_toLogoODI,  _ptoLogoODI,  CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
  LoadAndForceTexture(_toLogoEAX,  _ptoLogoEAX,  CTFILENAME("Textures\\Logo\\LogoEAX.tex"));
    /*
    // initialize and load corresponding fonts
    _fdSmall.Load_t(  CTFILENAME( "Fonts\\Display3-narrow.fnt"));
    _fdMedium.Load_t( CTFILENAME( "Fonts\\Display3-normal.fnt"));
    _fdBig.Load_t(    CTFILENAME( "Fonts\\Display3-caps.fnt"));
    _fdTitle.Load_t(  CTFILENAME( "Fonts\\Title2.fnt"));
    _fdSmall.SetCharSpacing(-1);
    _fdSmall.SetLineSpacing( 0);
    _fdSmall.SetSpaceWidth(0.4f);
    _fdMedium.SetCharSpacing(+1);
    _fdMedium.SetLineSpacing( 0);
    _fdMedium.SetSpaceWidth(0.4f);
    _fdBig.SetCharSpacing(+1);
    _fdBig.SetLineSpacing( 0);
    _fdTitle.SetCharSpacing(+1);
    _fdTitle.SetLineSpacing( 0);

    // load menu sounds
    _psdSelect = _pSoundStock->Obtain_t( CTFILENAME("Sounds\\Menu\\Select.wav"));
    _psdPress  = _pSoundStock->Obtain_t( CTFILENAME("Sounds\\Menu\\Press.wav"));
    _psoMenuSound = new CSoundObject;
*/
    // initialize and load menu textures
    _toPointer.SetData_t( CTFILENAME( "Textures\\General\\Pointer.tex"));
#if _SE_DEMO || TECHTESTONLY
    _toLogoMenuA.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256a_demo.tex"));
    _toLogoMenuB.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256b_demo.tex"));
#else
    _toLogoMenuA.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256a.tex"));
    _toLogoMenuB.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256b.tex"));
#endif
  }
  catch( char *strError) {
    FatalError( strError);
  }
  // force logo textures to be of maximal size
  ((CTextureData*)_toLogoMenuA.GetData())->Force(TEX_CONSTANT);
  ((CTextureData*)_toLogoMenuB.GetData())->Force(TEX_CONSTANT);
/*
  // menu's relative placement
  //CPlacement3D plRelative = CPlacement3D( FLOAT3D( 0.0f, 0.0f, -9.0f),
  //                          ANGLE3D( AngleDeg(0.0f), AngleDeg(0.0f), AngleDeg(0.0f)));
  try
  {
    TRANSLATERADIOARRAY(astrNoYes);
    TRANSLATERADIOARRAY(astrComputerInvoke);
    TRANSLATERADIOARRAY(astrDisplayAPIRadioTexts);
    TRANSLATERADIOARRAY(astrDisplayPrefsRadioTexts);
    TRANSLATERADIOARRAY(astrBitsPerPixelRadioTexts);
    TRANSLATERADIOARRAY(astrFrequencyRadioTexts);
    TRANSLATERADIOARRAY(astrSoundAPIRadioTexts);
    TRANSLATERADIOARRAY(astrDifficultyRadioTexts);
    TRANSLATERADIOARRAY(astrMaxPlayersRadioTexts);
    TRANSLATERADIOARRAY(astrWeapon);
    TRANSLATERADIOARRAY(astrSplitScreenRadioTexts);

    // initialize game type strings table
    InitGameTypes();

    // ------------------- Initialize menus
    gmConfirmMenu.Initialize_t();
    gmConfirmMenu.gm_strName="Confirm";
    gmConfirmMenu.gm_pmgSelectedByDefault = &mgConfirmYes;
    gmConfirmMenu.gm_pgmParentMenu = NULL;

    gmMainMenu.Initialize_t();
    gmMainMenu.gm_strName="Main";
    gmMainMenu.gm_pmgSelectedByDefault = &mgMainSingle;
    gmMainMenu.gm_pgmParentMenu = NULL;

    gmInGameMenu.Initialize_t();
    gmInGameMenu.gm_strName="InGame";
    gmInGameMenu.gm_pmgSelectedByDefault = &mgInGameQuickLoad;
    gmInGameMenu.gm_pgmParentMenu = NULL;

    gmSinglePlayerMenu.Initialize_t();
    gmSinglePlayerMenu.gm_strName="SinglePlayer";
    gmSinglePlayerMenu.gm_pmgSelectedByDefault = &mgSingleNewGame;
    gmSinglePlayerMenu.gm_pgmParentMenu = &gmMainMenu;

    gmSinglePlayerNewMenu.Initialize_t();
    gmSinglePlayerNewMenu.gm_strName="SinglePlayerNew";
    gmSinglePlayerNewMenu.gm_pmgSelectedByDefault = &mgSingleNewMedium;
    gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;

    gmDisabledFunction.Initialize_t();
    gmDisabledFunction.gm_strName="DisabledFunction";
    gmDisabledFunction.gm_pmgSelectedByDefault = &mgDisabledMenuButton;
    gmDisabledFunction.gm_pgmParentMenu = NULL;

    gmPlayerProfile.Initialize_t();
    gmPlayerProfile.gm_strName="PlayerProfile";
    gmPlayerProfile.gm_pmgSelectedByDefault = &mgPlayerName;

    gmControls.Initialize_t();
    gmControls.gm_strName="Controls";
    gmControls.gm_pmgSelectedByDefault = &mgControlsButtons;

    // warning! parent menu has to be set inside button activate function from where
    // Load/Save menu is called
    gmLoadSaveMenu.Initialize_t();
    gmLoadSaveMenu.gm_strName="LoadSave";
    gmLoadSaveMenu.gm_pmgSelectedByDefault = &amgLSButton[0];

    gmHighScoreMenu.Initialize_t();
    gmHighScoreMenu.gm_strName="HighScore";
    gmHighScoreMenu.gm_pmgSelectedByDefault = &mgBack;

    gmCustomizeKeyboardMenu.Initialize_t();
    gmCustomizeKeyboardMenu.gm_strName="CustomizeKeyboard";
    gmCustomizeKeyboardMenu.gm_pmgSelectedByDefault = &mgKey[0];
    gmCustomizeKeyboardMenu.gm_pgmParentMenu = &gmControls;

    gmCustomizeAxisMenu.Initialize_t();
    gmCustomizeAxisMenu.gm_strName="CustomizeAxis";
    gmCustomizeAxisMenu.gm_pmgSelectedByDefault = &mgAxisActionTrigger;
    gmCustomizeAxisMenu.gm_pgmParentMenu = &gmControls;

    gmOptionsMenu.Initialize_t();
    gmOptionsMenu.gm_strName="Options";
    gmOptionsMenu.gm_pmgSelectedByDefault = &mgVideoOptions;
    gmOptionsMenu.gm_pgmParentMenu = &gmMainMenu;

    gmVideoOptionsMenu.Initialize_t();
    gmVideoOptionsMenu.gm_strName="VideoOptions";
    gmVideoOptionsMenu.gm_pmgSelectedByDefault = &mgDisplayAPITrigger;
    gmVideoOptionsMenu.gm_pgmParentMenu = &gmOptionsMenu;

    gmAudioOptionsMenu.Initialize_t();
    gmAudioOptionsMenu.gm_strName="AudioOptions";
    gmAudioOptionsMenu.gm_pmgSelectedByDefault = &mgFrequencyTrigger;
    gmAudioOptionsMenu.gm_pgmParentMenu = &gmOptionsMenu;

    gmLevelsMenu.Initialize_t();
    gmLevelsMenu.gm_strName="Levels";
    gmLevelsMenu.gm_pmgSelectedByDefault = &mgManualLevel[0];
    gmLevelsMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;

    gmVarMenu.Initialize_t();
    gmVarMenu.gm_strName="Var";
    gmVarMenu.gm_pmgSelectedByDefault = &mgVar[0];
    gmVarMenu.gm_pgmParentMenu = &gmNetworkStartMenu;

    gmServersMenu.Initialize_t();
    gmServersMenu.gm_strName="Servers";
    gmServersMenu.gm_pmgSelectedByDefault = &mgServerList;
    gmServersMenu.gm_pgmParentMenu = &gmNetworkOpenMenu;

    gmNetworkMenu.Initialize_t();
    gmNetworkMenu.gm_strName="Network";
    gmNetworkMenu.gm_pmgSelectedByDefault = &mgNetworkJoin;
    gmNetworkMenu.gm_pgmParentMenu = &gmMainMenu;

    gmNetworkStartMenu.Initialize_t();
    gmNetworkStartMenu.gm_strName="NetworkStart";
    gmNetworkStartMenu.gm_pmgSelectedByDefault = &mgNetworkStartStart;
    gmNetworkStartMenu.gm_pgmParentMenu = &gmNetworkMenu;
    
    gmNetworkJoinMenu.Initialize_t();
    gmNetworkJoinMenu.gm_strName="NetworkJoin";
    gmNetworkJoinMenu.gm_pmgSelectedByDefault = &mgNetworkJoinLAN;
    gmNetworkJoinMenu.gm_pgmParentMenu = &gmNetworkMenu;

    gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
    gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
    gmSelectPlayersMenu.Initialize_t();
    gmSelectPlayersMenu.gm_strName="SelectPlayers";
    gmSelectPlayersMenu.gm_pmgSelectedByDefault = &mgSelectPlayersStart;

    gmNetworkOpenMenu.Initialize_t();
    gmNetworkOpenMenu.gm_strName="NetworkOpen";
    gmNetworkOpenMenu.gm_pmgSelectedByDefault = &mgNetworkOpenJoin;
    gmNetworkOpenMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;

    gmSplitScreenMenu.Initialize_t();
    gmSplitScreenMenu.gm_strName="SplitScreen";
    gmSplitScreenMenu.gm_pmgSelectedByDefault = &mgSplitScreenStart;
    gmSplitScreenMenu.gm_pgmParentMenu = &gmMainMenu;

    gmSplitStartMenu.Initialize_t();
    gmSplitStartMenu.gm_strName="SplitStart";
    gmSplitStartMenu.gm_pmgSelectedByDefault = &mgSplitStartStart;
    gmSplitStartMenu.gm_pgmParentMenu = &gmSplitScreenMenu;
  }
  catch( char *strError)
  {
    FatalError( strError);
  }
  */
}

BOOL SEMenu::run()
{
  
/*
  MenuUpdateMouseFocus();

  // if in fullscreen
  CDisplayMode dmCurrent;
  _pGfx->GetCurrentDisplayMode(dmCurrent);
  if (dmCurrent.IsFullScreen()) {
    // clamp mouse pointer
    _pixCursorPosI = Clamp(_pixCursorPosI, 0, dpMenu.GetWidth());
    _pixCursorPosJ = Clamp(_pixCursorPosJ, 0, dpMenu.GetHeight());
  // if in window
  } else {
    // use same mouse pointer as windows
    _pixCursorPosI = _pixCursorExternPosI;
    _pixCursorPosJ = _pixCursorExternPosJ;
  }

  pgmCurrentMenu->Think();

  TIME tmTickNow = _pTimer->GetRealTimeTick();

  while( _tmMenuLastTickDone<tmTickNow)
  {
    _pTimer->SetCurrentTick(_tmMenuLastTickDone);
    // call think for all gadgets in menu
    FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
      itmg->Think();
    }
    _tmMenuLastTickDone+=_pTimer->TickQuantum;
  }

  SetMenuLerping();
*/


  // blend background if menu is on
  if(active)
  {
    // get current time
    //TIME  tmNow = _pTimer->GetLerpedCurrentTick();
    //UBYTE ubH1  = (INDEX)(tmNow*08.7f) & 255;
    //UBYTE ubH2  = (INDEX)(tmNow*27.6f) & 255;
    //UBYTE ubH3  = (INDEX)(tmNow*16.5f) & 255;
    //UBYTE ubH4  = (INDEX)(tmNow*35.4f) & 255;
/*
    // clear screen with background texture
    _pGame->LCDPrepare(1.0f);
    _pGame->LCDSetDrawport(&dpMenu);
    // do not allow game to show through
    dpMenu.Fill(C_BLACK|255);
    _pGame->LCDRenderClouds1();
    _pGame->LCDRenderGrid();
    _pGame->LCDRenderClouds2();
*/
    // put logo(s) to main menu (if logos exist)
    /*
    if( pgmCurrentMenu==&gmMainMenu)
    {*/
    render->putTexture(_ptoLogoODI, render->getVirtX() - 50 - 16, render->getVirtY() - 16 - 50, 50, 50);
    render->putTexture(_ptoLogoCT, 16, render->getVirtY() - 16 - 50, 50, 50);

    PIX pixCenter = (PIX) (render->getVirtX()/2);
    render->putTexture(&_toLogoMenuA, pixCenter-256, 10, 256, 64);
    render->putTexture(&_toLogoMenuB, pixCenter, 10, 256, 64);

    /*} else if (pgmCurrentMenu==&gmAudioOptionsMenu) {
      if( _ptoLogoEAX!=NULL) {
        CTextureData &td = (CTextureData&)*_ptoLogoEAX->GetData();
        const INDEX iSize = 95;
        const PIX pixLogoWidth  = (PIX) (iSize * dpMenu.dp_fWideAdjustment);
        const PIX pixLogoHeight = (PIX) (iSize * td.GetHeight() / td.GetWidth());
        pixI0 =  (PIX) ((640-pixLogoWidth - 35)*fScaleW);
        pixJ0 = (PIX) ((480-pixLogoHeight - 7)*fScaleH);
        pixI1 = (PIX) (pixI0+ pixLogoWidth *fScaleW);
        pixJ1 = (PIX) (pixJ0+ pixLogoHeight*fScaleH);
        dpMenu.PutTexture( _ptoLogoEAX, PIXaabbox2D( PIX2D( pixI0, pixJ0),PIX2D( pixI1, pixJ1)));
      }
    }

#define THUMBW 96
#define THUMBH 96
    // if there is a thumbnail
    if( _bThumbnailOn) {
      const FLOAT fThumbScaleW = fScaleW * dpMenu.dp_fWideAdjustment;
      PIX pixOfs = (PIX) (8*fScaleW);
      pixI0 = (PIX) (8*fScaleW);
      pixJ0 = (PIX) ((240-THUMBW/2)*fScaleH);
      pixI1 = (PIX) (pixI0+ THUMBW*fThumbScaleW);
      pixJ1 = (PIX) (pixJ0+ THUMBH*fScaleH);
      if( _toThumbnail.GetData()!=NULL)
      { // show thumbnail with shadow and border
        dpMenu.Fill( pixI0+pixOfs, pixJ0+pixOfs, THUMBW*fThumbScaleW, THUMBH*fScaleH, C_BLACK|128);
        dpMenu.PutTexture( &_toThumbnail, PIXaabbox2D( PIX2D( pixI0, pixJ0), PIX2D( pixI1, pixJ1)), C_WHITE|255);
        dpMenu.DrawBorder( pixI0,pixJ0, THUMBW*fThumbScaleW,THUMBH*fScaleH, _pGame->LCDGetColor(C_mdGREEN|255, "thumbnail border"));
      } else {
        dpMenu.SetFont( _pfdDisplayFont);
        dpMenu.SetTextScaling( fScaleW);
        dpMenu.SetTextAspect( 1.0f);
        dpMenu.PutTextCXY( TRANS("no thumbnail"), (pixI0+pixI1)/2, (pixJ0+pixJ1)/2, _pGame->LCDGetColor(C_GREEN|255, "no thumbnail"));
      }
    }

    // assure we can listen to non-3d sounds
    _pSound->UpdateSounds();
  }

  // if this is popup menu
  if (pgmCurrentMenu->gm_bPopup) {

    // render parent menu first
    if (pgmCurrentMenu->gm_pgmParentMenu!=NULL) {
      _pGame->MenuPreRenderMenu(pgmCurrentMenu->gm_pgmParentMenu->gm_strName);
      FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_pgmParentMenu->gm_lhGadgets, itmg) {
        if( itmg->mg_bVisible) {
          itmg->Render( &dpMenu);
        }
      }
      _pGame->MenuPostRenderMenu(pgmCurrentMenu->gm_pgmParentMenu->gm_strName);
    }
*/
    // gray it out
    //dpMenu.Fill(C_BLACK|128);
/*
    // clear popup box
    dpMenu.Unlock();
    PIXaabbox2D box = FloatBoxToPixBox(&dpMenu, BoxPopup());
    CDrawPort dpPopup(pdp, box);
    dpPopup.Lock();
    _pGame->LCDSetDrawport(&dpPopup);
    dpPopup.Fill(C_BLACK|255);
    _pGame->LCDRenderClouds1();
    _pGame->LCDRenderGrid();
  //_pGame->LCDRenderClouds2();
    _pGame->LCDScreenBox(_pGame->LCDGetColor(C_GREEN|255, "popup box"));
    dpPopup.Unlock();
    dpMenu.Lock();
  }

  // no entity is under cursor initially
  _pmgUnderCursor = NULL;

  BOOL bStillInMenus = FALSE;
  _pGame->MenuPreRenderMenu(pgmCurrentMenu->gm_strName);
  // for each menu gadget
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if gadget is visible
    if( itmg->mg_bVisible) {
      bStillInMenus = TRUE;
      itmg->Render( &dpMenu);
      if (FloatBoxToPixBox(&dpMenu, itmg->mg_boxOnScreen)>=PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
        _pmgUnderCursor = itmg;
      }
    }
  }
  _pGame->MenuPostRenderMenu(pgmCurrentMenu->gm_strName);

  // no currently active gadget initially
  CMenuGadget *pmgActive = NULL;
  // if mouse was not active last
  if (!_bMouseUsedLast) {
    // find focused gadget
    FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
      //CMenuGadget &mg = *itmg;
      // if focused
      if( itmg->mg_bFocused) {
        // it is active
        pmgActive = &itmg.Current();
        break;
      }
    }
  // if mouse was active last
  } else {
    // gadget under cursor is active
    pmgActive = _pmgUnderCursor;
  }

  // if editing
  if (_bEditingString && pmgActive!=NULL) {
    // dim the menu  bit
    dpMenu.Fill(C_BLACK|0x40);
    // render the edit gadget again
    pmgActive->Render(&dpMenu);
  }
  
  // if there is some active gadget and it has tips
  if (pmgActive!=NULL && (pmgActive->mg_strTip!="" || _bEditingString)) {
    CTString strTip = pmgActive->mg_strTip;
    if (_bEditingString) {
      strTip = TRANS("Enter - OK, Escape - Cancel");
    }
    // print the tip
    SetFontMedium(&dpMenu);
    dpMenu.PutTextC(strTip, 
      pixW*0.5f, pixH*0.92f, _pGame->LCDGetColor(C_WHITE|255, "tool tip"));
      */
  }
/*
  _pGame->ConsolePrintLastLines(&dpMenu);

  RenderMouseCursor(&dpMenu);
*/
/*
  return bStillInMenus;
  */
 return TRUE;
}
/*

void SEMenu::LCDPrepare(FLOAT fFade)
{
  // get current time and alpha value
  _tmNow_SE = (FLOAT)_pTimer->GetHighPrecisionTimer().GetSeconds();
  _ulA_SE   = NormFloatToByte(fFade);

  ::_LCDPrepare(fFade);
}
void SEMenu::LCDSetDrawport(CDrawPort *pdp)
{
  _pdp_SE = pdp;
  _pixSizeI_SE = _pdp_SE->GetWidth();
  _pixSizeJ_SE = _pdp_SE->GetHeight();
  _boxScreen_SE = PIXaabbox2D ( PIX2D(0,0), PIX2D(_pixSizeI_SE, _pixSizeJ_SE));
    
  if (pdp->dp_SizeIOverRasterSizeI==1.0f) {
    _bPopup = FALSE;
  } else {
    _bPopup = TRUE;
  }
  
  ::_LCDSetDrawport(pdp);
}
void SEMenu::LCDDrawBox(PIX pixUL, PIX pixDR, const PIXaabbox2D &box, COLOR col)
{
  col = SE_COL_BLUE_NEUTRAL|255;

  ::_LCDDrawBox(pixUL, pixDR, box, col);
}
void SEMenu::LCDScreenBox(COLOR col)
{
  col = SE_COL_BLUE_NEUTRAL|255;

  ::_LCDScreenBox(col);
}
void SEMenu::LCDScreenBoxOpenLeft(COLOR col)
{
  col = SE_COL_BLUE_NEUTRAL|255;

  ::_LCDScreenBoxOpenLeft(col);
}
void SEMenu::LCDScreenBoxOpenRight(COLOR col)
{
  col = SE_COL_BLUE_NEUTRAL|255;

  ::_LCDScreenBoxOpenRight(col);
}
void SEMenu::LCDRenderClouds1(void)
{
  _pdp_SE->PutTexture(&_toBackdrop, _boxScreen_SE, C_WHITE|255);

  if (!_bPopup) {

    PIXaabbox2D box;
        
    // right character - Sam
    INDEX iSize = 170;
    INDEX iYU = 120;
    INDEX iYM = iYU + iSize;
    INDEX iYB = iYM + iSize;
    INDEX iXL = 420;
    INDEX iXR = (INDEX) (iXL + iSize*_pdp_SE->dp_fWideAdjustment);
    
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYU*_pdp_SE->GetHeight()/480) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetHeight()/480));
    _pdp_SE->PutTexture(&_toSamU, box, SE_COL_BLUE_NEUTRAL|255);
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetHeight()/480) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYB*_pdp_SE->GetHeight()/480));
    _pdp_SE->PutTexture(&_toSamD, box, SE_COL_BLUE_NEUTRAL|255);

    iSize = 120;
    iYU = 0;
    iYM = iYU + iSize;
    iYB = iYM + iSize;
    iXL = -20;
    iXR = iXL + iSize;
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYU*_pdp_SE->GetWidth()/640) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetWidth()/640));
    _pdp_SE->PutTexture(&_toLeftU, box, SE_COL_BLUE_NEUTRAL|200);
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetWidth()/640) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYB*_pdp_SE->GetWidth()/640));
    _pdp_SE->PutTexture(&_toLeftD, box, SE_COL_BLUE_NEUTRAL|200);
    iYU = iYB;
    iYM = iYU + iSize;
    iYB = iYM + iSize;
    iXL = -20;
    iXR = iXL + iSize;
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYU*_pdp_SE->GetWidth()/640) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetWidth()/640));
    _pdp_SE->PutTexture(&_toLeftU, box, SE_COL_BLUE_NEUTRAL|200);
    box = PIXaabbox2D( PIX2D( iXL*_pdp_SE->GetWidth()/640, iYM*_pdp_SE->GetWidth()/640) ,
                       PIX2D( iXR*_pdp_SE->GetWidth()/640, iYB*_pdp_SE->GetWidth()/640));
    _pdp_SE->PutTexture(&_toLeftD, box, SE_COL_BLUE_NEUTRAL|200);
  
  }

  MEXaabbox2D boxBcgClouds1;
  TiledTextureSE(_boxScreen_SE, 1.2f*_pdp_SE->GetWidth()/640.0f, 
    MEX2D(sin(_tmNow_SE*0.5f)*35,sin(_tmNow_SE*0.7f+1)*21),   boxBcgClouds1);
  _pdp_SE->PutTexture(&_toBcgClouds, _boxScreen_SE, boxBcgClouds1, C_BLACK|_ulA_SE>>2);
  TiledTextureSE(_boxScreen_SE, 0.7f*_pdp_SE->GetWidth()/640.0f, 
    MEX2D(sin(_tmNow_SE*0.6f+1)*32,sin(_tmNow_SE*0.8f)*25),   boxBcgClouds1);
  _pdp_SE->PutTexture(&_toBcgClouds, _boxScreen_SE, boxBcgClouds1, C_BLACK|_ulA_SE>>2);
}
void SEMenu::LCDRenderCloudsForComp(void)
{
  MEXaabbox2D boxBcgClouds1;
  TiledTextureSE(_boxScreen_SE, 1.856f*_pdp_SE->GetWidth()/640.0f, 
    MEX2D(sin(_tmNow_SE*0.5f)*35,sin(_tmNow_SE*0.7f)*21),   boxBcgClouds1);
  _pdp_SE->PutTexture(&_toBcgClouds, _boxScreen_SE, boxBcgClouds1, SE_COL_BLUE_NEUTRAL|_ulA_SE>>2);
  TiledTextureSE(_boxScreen_SE, 1.323f*_pdp_SE->GetWidth()/640.0f, 
    MEX2D(sin(_tmNow_SE*0.6f)*31,sin(_tmNow_SE*0.8f)*25),   boxBcgClouds1);
  _pdp_SE->PutTexture(&_toBcgClouds, _boxScreen_SE, boxBcgClouds1, SE_COL_BLUE_NEUTRAL|_ulA_SE>>2);
}
void SEMenu::LCDRenderClouds2(void)
{
  NOTHING;
}
void SEMenu::LCDRenderGrid(void)
{
  NOTHING;
}
void SEMenu::LCDRenderCompGrid(void)
{
   MEXaabbox2D boxBcgGrid;
   TiledTextureSE(_boxScreen_SE, 0.5f*_pdp_SE->GetWidth()/(_pdp_SE->dp_SizeIOverRasterSizeI*640.0f), MEX2D(0,0), boxBcgGrid);
   _pdp_SE->PutTexture(&_toBcgGrid, _boxScreen_SE, boxBcgGrid, SE_COL_BLUE_NEUTRAL|_ulA_SE>>1); 
}
void SEMenu::LCDDrawPointer(PIX pixI, PIX pixJ)
{
  CDisplayMode dmCurrent;
  _pGfx->GetCurrentDisplayMode(dmCurrent);
  if (dmCurrent.IsFullScreen()) {
    while (ShowCursor(FALSE) >= 0);
  } else {
    if (!_pInput->IsInputEnabled()) {
      while (ShowCursor(TRUE) < 0);
    }
    return;
  }
  PIX pixSizeI = _toPointer.GetWidth();
  PIX pixSizeJ = _toPointer.GetHeight();
  pixI-=1;
  pixJ-=1;
  _pdp_SE->PutTexture( &_toPointer, PIXaabbox2D( PIX2D(pixI, pixJ), PIX2D(pixI+pixSizeI, pixJ+pixSizeJ)),
                    LCDFadedColor(C_WHITE|255));

  //::_LCDDrawPointer(pixI, pixJ);
}*/