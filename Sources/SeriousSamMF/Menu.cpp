#include "Menu.h"
#include <Engine/Base/ErrorReporting.h> /* FatalError() */
#include <Engine/Graphics/ViewPort.h> /* CViewPort */
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */
#include <Engine/Math/AABBox.h> /* PIXaabbox2D */
#include <Engine/Base/ListIterator.inl>
#include <Engine/Graphics/Font.h>

#include "Colors.h"

CFontData _fdBig;
CFontData _fdMedium;
CFontData _fdSmall;
CFontData _fdTitle;
/*
// ------------------------ SGameMenu implementation
CGameMenu::CGameMenu( void)
{
  gm_pgmParentMenu = NULL;
  gm_pmgSelectedByDefault = NULL;
  gm_pmgArrowUp = NULL;
  gm_pmgArrowDn = NULL;
  gm_pmgListTop = NULL;
  gm_pmgListBottom = NULL;
  gm_iListOffset = 0;
  gm_ctListVisible = 0;
  gm_ctListTotal = 0;
  gm_bPopup = FALSE;
}

void CGameMenu::Initialize_t( void)
{
}

void CGameMenu::Destroy(void)
{
}
void CGameMenu::FillListItems(void)
{
  ASSERT(FALSE);  // must be implemented to scroll up/down
}

// +-1 -> hit top/bottom when pressing up/down on keyboard
// +-2 -> pressed pageup/pagedown on keyboard
// +-3 -> pressed arrow up/down  button in menu
// +-4 -> scrolling with mouse wheel
void CGameMenu::ScrollList(INDEX iDir)
{
  // if not valid for scrolling
  if (gm_ctListTotal<=0
    || gm_pmgArrowUp == NULL || gm_pmgArrowDn == NULL
    || gm_pmgListTop == NULL || gm_pmgListBottom == NULL) {
    // do nothing
    return;
  }

  INDEX iOldTopKey = gm_iListOffset;
  // change offset
  switch(iDir) {
    case -1:
      gm_iListOffset -= 1;
      break;
    case -4:
      gm_iListOffset -= 3;
      break;
    case -2:
    case -3:
      gm_iListOffset -= gm_ctListVisible;
      break;
    case +1:
      gm_iListOffset += 1;
      break;
    case +4:
      gm_iListOffset += 3;
      break;
    case +2:
    case +3:
      gm_iListOffset += gm_ctListVisible;
      break;
    default:
      ASSERT(FALSE);
      return;
  }
  if (gm_ctListTotal<=gm_ctListVisible) {
    gm_iListOffset = 0;
  } else {
    gm_iListOffset = Clamp(gm_iListOffset, INDEX(0), INDEX(gm_ctListTotal-gm_ctListVisible));
  }

  // set new names
  FillListItems();

  // if scroling with wheel
  if (iDir==+4 || iDir==-4) {
    // no focus changing
    return;
  }

  // delete all focuses
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    itmg->OnKillFocus();
  }

  // set new focus
  //const INDEX iFirst = 0;
  //const INDEX iLast = gm_ctListVisible-1;
  switch(iDir) {
    case +1:
      gm_pmgListBottom->OnSetFocus();
      break;
    case +2:
      if (gm_iListOffset!=iOldTopKey) {
        gm_pmgListTop->OnSetFocus();
      } else {
        gm_pmgListBottom->OnSetFocus();
      }
    break;
    case +3:
      gm_pmgArrowDn->OnSetFocus();
      break;
    case -1:
      gm_pmgListTop->OnSetFocus();
      break;
    case -2:
      gm_pmgListTop->OnSetFocus();
      break;
    case -3:
      gm_pmgArrowUp->OnSetFocus();
      break;
  }
}

void CGameMenu::KillAllFocuses(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
    itmg->mg_bFocused = FALSE;
  }
}

void CGameMenu::StartMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    itmg->mg_bFocused = FALSE;
    // call appear
    itmg->Appear();
  }

  // if there is a list
  if (gm_pmgListTop!=NULL) {
    // scroll it so that the wanted tem is centered
    gm_iListOffset = gm_iListWantedItem-gm_ctListVisible/2;
    // clamp the scrolling
    gm_iListOffset = Clamp(gm_iListOffset, 0, Max(0, gm_ctListTotal-gm_ctListVisible));

    // fill the list
    FillListItems();

    // for each menu gadget in menu
    FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
      // if in list, but disabled
      if (itmg->mg_iInList==-2) {
        // hide it
        itmg->mg_bVisible = FALSE;
      // if in list
      } else if (itmg->mg_iInList>=0) {
        // show it
        itmg->mg_bVisible = TRUE;
      }
      // if wanted
      if (itmg->mg_iInList==gm_iListWantedItem) {
        // focus it
        itmg->OnSetFocus();
        gm_pmgSelectedByDefault = itmg;
      }
    }
  }
}

void CGameMenu::EndMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    // call disappear
    itmg->Disappear();
  }
}

// return TRUE if handled
BOOL CGameMenu::OnKeyDown( int iVKey)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if( itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if( pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if( pmgActive->OnKeyDown( iVKey)) {
    // key is handled
    return TRUE;
  }

  // process normal in menu movement
  switch( iVKey) {
  case VK_PRIOR:
    ScrollList(-2);
    return TRUE;
  case VK_NEXT:
    ScrollList(+2);
    return TRUE;

  case 11:
    ScrollList(-4);
    return TRUE;
  case 10:
    ScrollList(+4);
    return TRUE;

  case VK_UP:
    // if this is top button in list
    if (pmgActive==gm_pmgListTop) {
      // scroll list up
      ScrollList(-1);
      // key is handled
      return TRUE;
    }
    // if we can go up
    if(pmgActive->mg_pmgUp != NULL && pmgActive->mg_pmgUp->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgUp;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_DOWN:
    // if this is bottom button in list
    if (pmgActive==gm_pmgListBottom) {
      // scroll list down
      ScrollList(+1);
      // key is handled
      return TRUE;
    }
    // if we can go down
    if(pmgActive->mg_pmgDown != NULL && pmgActive->mg_pmgDown->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgDown;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_LEFT:
    // if we can go left
    if(pmgActive->mg_pmgLeft != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgLeft->mg_bVisible && gm_pmgSelectedByDefault!=NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      } else {
        pmgActive = pmgActive->mg_pmgLeft;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_RIGHT:
    // if we can go right
    if(pmgActive->mg_pmgRight != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgRight->mg_bVisible && gm_pmgSelectedByDefault!=NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      } else {
        pmgActive = pmgActive->mg_pmgRight;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  }

  // key is not handled
  return FALSE;
}

void CGameMenu::Think(void)
{
}

BOOL CGameMenu::OnChar(MSG msg)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if( itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if( pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if( pmgActive->OnChar(msg)) {
    // key is handled
    return TRUE;
  }

  // key is not handled
  return FALSE;
}
*/
static const FLOAT _fBigStartJ = 0.25f;
static const FLOAT _fBigSizeJ = 0.066f;
static const FLOAT _fMediumSizeJ = 0.04f;

static const FLOAT _fNoStartI = 0.25f;
static const FLOAT _fNoSizeI = 0.04f;
static const FLOAT _fNoSpaceI = 0.01f;
static const FLOAT _fNoUpStartJ = 0.24f;
static const FLOAT _fNoDownStartJ = 0.44f;
static const FLOAT _fNoSizeJ = 0.04f;

FLOATaabbox2D BoxVersion(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+-5.5f*_fMediumSizeJ),
    FLOAT2D(0.97f, _fBigStartJ+(-5.5f+1)*_fMediumSizeJ));
}
FLOATaabbox2D BoxMediumRow(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.95f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxBigRow(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.1f, _fBigStartJ+fRow*_fBigSizeJ),
    FLOAT2D(0.9f, _fBigStartJ+(fRow+1)*_fBigSizeJ));
}

PIXaabbox2D CMainMenu::positionMenuItem(const SERender* render, FLOAT row)
{
  return render->box2D(0.0f, 
                      positionStart + row * fontSize, 
                      1.0f, 
                      positionStart + (row + 1) * fontSize
  );
}
/*
void MenuUpdateMouseFocus(void)
{
  // get real cursor position
  POINT pt;
  GetCursorPos(&pt);
  extern INDEX sam_bWideScreen;
  extern CDrawPort *pdp;
  if( sam_bWideScreen) {
    const PIX pixHeight = pdp->GetHeight();
    pt.y -= (LONG) ((pixHeight/0.75f-pixHeight)/2);
  }
  _pixCursorPosI += pt.x-_pixCursorExternPosI;
  _pixCursorPosJ  = _pixCursorExternPosJ;
  _pixCursorExternPosI = pt.x;
  _pixCursorExternPosJ = pt.y;

  // if mouse not used last
  if (!_bMouseUsedLast||_bDefiningKey||_bEditingString) {
    // do nothing
    return;
  }

  CMenuGadget *pmgActive = NULL;
  // for all gadgets in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    //CMenuGadget &mg = *itmg;
    // if focused
    if( itmg->mg_bFocused) {
      // remember it
      pmgActive = &itmg.Current();
    }
  }

  // if there is some under cursor
  if (_pmgUnderCursor!=NULL) {
    _pmgUnderCursor->OnMouseOver(_pixCursorPosI, _pixCursorPosJ);
    // if the one under cursor has no neighbours
    if (_pmgUnderCursor->mg_pmgLeft ==NULL 
      &&_pmgUnderCursor->mg_pmgRight==NULL 
      &&_pmgUnderCursor->mg_pmgUp   ==NULL 
      &&_pmgUnderCursor->mg_pmgDown ==NULL) {
      // it cannot be focused
      _pmgUnderCursor = NULL;
      return;
    }

    // if the one under cursor is not active and not disappearing
    if (pmgActive!=_pmgUnderCursor && _pmgUnderCursor->mg_bVisible) {
      // change focus
      if (pmgActive!=NULL) {
        pmgActive->OnKillFocus();
      }
      _pmgUnderCursor->OnSetFocus();
    }
  }
}*/
// ------------------------ CMainMenu implementation
extern BOOL _bRunning;
void ExitGame()
{
  _bRunning = FALSE;
}

CMainMenu::CMainMenu()
{
    // intialize main menu
/*
  mgMainTitle.mg_strText = "SERIOUS SAM - BETA";  // nothing to see here, kazuya
  mgMainTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail( mgMainTitle.mg_lnNode);
  */
 positionStart = 0.25f;
 fontSize = 0.07f;
/*
  CTString sam_strVersion = "VERSIONE DA AGGIUNGERE";
  mgMainVersionLabel.mg_strText = sam_strVersion;
  mgMainVersionLabel.mg_boxOnScreen = BoxVersion();
  mgMainVersionLabel.mg_bfsFontSize = BFS_MEDIUM;
  mgMainVersionLabel.mg_iCenterI = +1;
  mgMainVersionLabel.mg_bEnabled = FALSE;
  mgMainVersionLabel.mg_bLabel = TRUE;
  gm_lhGadgets.AddTail( mgMainVersionLabel.mg_lnNode);

  CTString sam_strModName = "MOD DA AGGIUNGERE";
  mgMainModLabel.mg_strText = sam_strModName;
  mgMainModLabel.mg_boxOnScreen = BoxMediumRow(-2.0f);
  mgMainModLabel.mg_bfsFontSize = BFS_MEDIUM;
  mgMainModLabel.mg_iCenterI = 0;
  mgMainModLabel.mg_bEnabled = FALSE;
  mgMainModLabel.mg_bLabel = TRUE;
  gm_lhGadgets.AddTail( mgMainModLabel.mg_lnNode);
*/
  mgMainSingle.mg_strText = TRANS("SINGLE PLAYER");
  mgMainSingle.mg_bfsFontSize = BFS_LARGE;
  mgMainSingle.mg_strTip = TRANS("single player game menus");
  gm_lhGadgets.AddTail( mgMainSingle.mg_lnNode);
  mgMainSingle.mg_pmgUp = &mgMainQuit;
  mgMainSingle.mg_pmgDown = &mgMainNetwork;
  mgMainSingle.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainSingle.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainSingle.mg_bRectangle = TRUE;
  //mgMainSingle.mg_pActivatedFunction = &StartSinglePlayerMenu;

  mgMainNetwork.mg_strText = TRANS("NETWORK");
  mgMainNetwork.mg_bfsFontSize = BFS_LARGE;
  mgMainNetwork.mg_strTip = TRANS("LAN/iNet multiplayer menus");
  gm_lhGadgets.AddTail( mgMainNetwork.mg_lnNode);
  mgMainNetwork.mg_pmgUp = &mgMainSingle;
  mgMainNetwork.mg_pmgDown = &mgMainSplitScreen;
  mgMainNetwork.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainNetwork.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainNetwork.mg_bRectangle = TRUE;
  //mgMainNetwork.mg_pActivatedFunction = StartNetworkMenu;

  mgMainSplitScreen.mg_strText = TRANS("SPLIT SCREEN");
  mgMainSplitScreen.mg_bfsFontSize = BFS_LARGE;
  mgMainSplitScreen.mg_strTip = TRANS("play with multiple players on one computer");
  gm_lhGadgets.AddTail( mgMainSplitScreen.mg_lnNode);
  mgMainSplitScreen.mg_pmgUp = &mgMainNetwork;
  mgMainSplitScreen.mg_pmgDown = &mgMainDemo;
  mgMainSplitScreen.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainSplitScreen.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainSplitScreen.mg_pActivatedFunction = &StartSplitScreenMenu;

  mgMainDemo.mg_strText = TRANS("DEMO");
  mgMainDemo.mg_bfsFontSize = BFS_LARGE;
  mgMainDemo.mg_strTip = TRANS("play a game demo");
  gm_lhGadgets.AddTail( mgMainDemo.mg_lnNode);
  mgMainDemo.mg_pmgUp = &mgMainSplitScreen;
  mgMainDemo.mg_pmgDown = &mgMainMods;
  mgMainDemo.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainDemo.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainDemo.mg_pActivatedFunction = &StartDemoLoadMenu;

  mgMainMods.mg_strText = TRANS("MODS");
  mgMainMods.mg_bfsFontSize = BFS_LARGE;
  mgMainMods.mg_strTip = TRANS("run one of installed game modifications");
  gm_lhGadgets.AddTail( mgMainMods.mg_lnNode);
  mgMainMods.mg_pmgUp = &mgMainDemo;
  mgMainMods.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainMods.colSelected = SE_COL_ORANGE_DARK|255;
  mgMainMods.mg_pmgDown = &mgMainHighScore;

  mgMainHighScore.mg_strText = TRANS("HIGH SCORES");
  mgMainHighScore.mg_bfsFontSize = BFS_LARGE;
  mgMainHighScore.mg_strTip = TRANS("view list of top ten best scores");
  gm_lhGadgets.AddTail( mgMainHighScore.mg_lnNode);
  mgMainHighScore.mg_pmgUp = &mgMainMods;
  mgMainHighScore.mg_pmgDown = &mgMainOptions;
  mgMainHighScore.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainHighScore.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainHighScore.mg_pActivatedFunction = &StartHighScoreMenu;

  mgMainOptions.mg_strText = TRANS("OPTIONS");
  mgMainOptions.mg_bfsFontSize = BFS_LARGE;
  mgMainOptions.mg_strTip = TRANS("adjust video, audio and input options");
  gm_lhGadgets.AddTail( mgMainOptions.mg_lnNode);
  mgMainOptions.mg_pmgUp = &mgMainHighScore;
  mgMainOptions.mg_pmgDown = &mgMainQuit;
  mgMainOptions.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainOptions.colSelected = SE_COL_ORANGE_DARK|255;
  //mgMainOptions.mg_pActivatedFunction = &StartOptionsMenu;
  
  mgMainQuit.mg_strText = TRANS("QUIT");
  mgMainQuit.mg_bfsFontSize = BFS_LARGE;
  mgMainQuit.mg_strTip = TRANS("exit game immediately");
  gm_lhGadgets.AddTail( mgMainQuit.mg_lnNode);
  mgMainQuit.mg_pmgUp = &mgMainOptions;
  mgMainQuit.mg_pmgDown = &mgMainSingle;
  mgMainQuit.colEnable = SE_COL_ORANGE_LIGHT|255;
  mgMainQuit.colSelected = SE_COL_ORANGE_DARK|255;
  mgMainQuit.mg_pActivatedFunction = &ExitGame;
}

CMainMenu::~CMainMenu()
{

}
void CMainMenu::StartMenu(void)
{/*
  mgMainSingle.mg_bEnabled      = IsMenuEnabled("Single Player");
  mgMainNetwork.mg_bEnabled     = IsMenuEnabled("Network");
  mgMainSplitScreen.mg_bEnabled = IsMenuEnabled("Split Screen");
  mgMainHighScore.mg_bEnabled   = IsMenuEnabled("High Score");
  CGameMenu::StartMenu();*/
}
void CMainMenu::render(const SERender* render)
{
    mgMainSingle.mg_boxOnScreen = positionMenuItem(render, 0.0f);
    mgMainNetwork.mg_boxOnScreen = positionMenuItem(render, 1.0f);
    mgMainSplitScreen.mg_boxOnScreen = positionMenuItem(render, 2.0f);
    mgMainDemo.mg_boxOnScreen = positionMenuItem(render, 3.0f);
    mgMainMods.mg_boxOnScreen = positionMenuItem(render, 4.0f);
    mgMainHighScore.mg_boxOnScreen = positionMenuItem(render, 5.0f);
    mgMainOptions.mg_boxOnScreen = positionMenuItem(render, 6.0f);
    mgMainQuit.mg_boxOnScreen = positionMenuItem(render, 7.0f);

    FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
        itmg->render(render);
    }

}


void CMainMenu::update(const SDL_Event* event, POINT cursor)
{
    mgMainSingle.update(event, cursor);
    mgMainNetwork.update(event, cursor);
    mgMainSplitScreen.update(event, cursor);
    mgMainDemo.update(event, cursor);
    mgMainMods.update(event, cursor);
    mgMainHighScore.update(event, cursor);
    mgMainOptions.update(event, cursor);
    mgMainQuit.update(event, cursor);
}

SEMenu::SEMenu()
{
    active = FALSE;
}

SEMenu::~SEMenu()
{
    
}

void SEMenu::init()
{
    
  try {
      // load logo textures
    
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
/*
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
*/

/*
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

void SEMenu::render(const SERender* render)
{/*
  if(gmMainMenu == NULL) {
    gmMainMenu = new CMainMenu();
    gmMainMenu->init(render);
    gmMainMenu->gm_strName="Main";
    gmMainMenu->gm_pmgSelectedByDefault = &gmMainMenu->mgMainSingle;
    gmMainMenu->gm_pgmParentMenu = NULL;

    pgmCurrentMenu = gmMainMenu;
  }*/
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
  */
  // for each menu gadget
  /*
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if gadget is visible
    if( itmg->mg_bVisible) {
      //bStillInMenus = TRUE;
      itmg->render(render);
      if (FloatBoxToPixBox(&dpMenu, itmg->mg_boxOnScreen)>=PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
        _pmgUnderCursor = itmg;
      }
    }
  }
  */
  /*
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
 return;
}

void SEMenu::update(const SDL_Event* event, POINT cursor)
{

}