#include <stddef.h>

#include <SDL.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/Stream.h>
#include <Engine/Engine.h>
#include <Engine/Sound/SoundLibrary.h>
#include <Engine/Base/SDL/SDLEvents.h>

#include "SplashScreen.h"
#include "Game.h"
#include "Menu.h"
#include "MainWindow.h"
#include "InterfaceSDL.h"

static char *argv0 = NULL;

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
ENGINE_API extern INDEX snd_iFormat;

BOOL _bRunning = FALSE;
BOOL _bQuitScreen = FALSE;
BOOL sam_bFirstStarted = TRUE; /* FIXME: Check if is first start */

PIX  _pixDesktopWidth = 0;    // desktop width when started (for some tests)

CTString sam_strGameName = "serioussammf";

#ifdef PLATFORM_UNIX

#endif

SESplashScreen* scr_splashscreen = NULL;
SEGame* pGame = NULL;
SEMenu* pMenu = NULL;
SEMainWindow* pMainWin = NULL;
/* FIXME: Remove direct access to this interface! */
SEInterfaceSDL* pSDL = NULL;

// display mode settings
INDEX sam_bFullScreenActive = FALSE;
INDEX sam_iScreenSizeI = 640;  // current size of the window
INDEX sam_iScreenSizeJ = 480;  // current size of the window
INDEX sam_iDisplayDepth  = 0;  // 0==default, 1==16bit, 2==32bit
INDEX sam_iDisplayAdapter = 0; 
INDEX sam_iGfxAPI = 0;         // 0==OpenGL

// 0...app started for the first time
// 1...all ok
// 2...automatic fallback
static INDEX _iDisplayModeChangeFlag = 0;
static TIME _tmDisplayModeChanged = 100.0f; // when display mode was last changed


// list of possible display modes for recovery 
const INDEX aDefaultModes[][3] =
{ // color, API, adapter
  { DD_DEFAULT, GAT_OGL, 0},
  { DD_16BIT,   GAT_OGL, 0},
  { DD_16BIT,   GAT_OGL, 1}, // 3dfx Voodoo2
#ifdef SE1_D3D
  { DD_DEFAULT, GAT_D3D, 0},
  { DD_16BIT,   GAT_D3D, 0},
  { DD_16BIT,   GAT_D3D, 1},
#endif // SE1_D3D
};
const INDEX ctDefaultModes = ARRAYCOUNT(aDefaultModes);


// list of settings data
static CListHead _lhSettings;
extern INDEX sam_iVideoSetup;  // 0==speed, 1==normal, 2==quality, 3==custom

class CSettingsEntry {
public:
   CListNode se_lnNode;
    CTString se_strRenderer;
    CTString se_strDescription;
  CTFileName se_fnmScript;
  // check if this entry matches given info
  BOOL Matches( const CTString &strRenderer) const;
};

// last valid settings info
static CTString _strLastRenderer;
CTString _strPreferencesDescription = "";
INDEX    _iLastPreferences = 1;

// check if this entry matches given info
BOOL CSettingsEntry::Matches( const CTString &strRenderer) const
{
  return strRenderer.Matches(se_strRenderer);
}


void InitGLSettings()
{
  ASSERT(_lhSettings.IsEmpty());

  char achrLine    [1024];
  char achrRenderer[1024];
  char achrDesc    [1024];
  char achrScript  [1024];

  CTFileStream strmFile;
  try
  {
    strmFile.Open_t( CTString("Scripts\\GLSettings\\GLSettings.lst"), CTStream::OM_READ);
    //INDEX iIndex = 0;
	  do
    {
      achrLine    [0] = 0;
      achrRenderer[0] = 0;
      achrDesc    [0] = 0;
      achrScript  [0] = 0;
      strmFile.GetLine_t( achrLine, 1024);
      sscanf( achrLine,
        "\"%1024[^\"]\"%*[^\"]\"%1024[^\"]\"%*[^\"]\"%1024[^\"]\"", achrRenderer, achrDesc, achrScript);
      if( achrRenderer[0]==0) continue;

      CSettingsEntry &se = *new CSettingsEntry;
      se.se_strRenderer    = achrRenderer;
      se.se_strDescription = achrDesc;
      se.se_fnmScript      = CTString(achrScript);
      _lhSettings.AddTail( se.se_lnNode);
    }
  	while( !strmFile.AtEOF());
  }

  // ignore errors
  catch (char *strError) { 
    WarningMessage(TRANS("unable to setup OpenGL settings list: %s"), strError);
  }

  _strLastRenderer= "none";
  _iLastPreferences = 1;
  /*
  _pShell->DeclareSymbol("persistent CTString sam_strLastRenderer;", (void *) &_strLastRenderer);
  _pShell->DeclareSymbol("persistent INDEX    sam_iLastSetup;",      (void *) &_iLastPreferences);
  */
}

BOOL _bWindowChanging = FALSE;    // ignores window messages while this is set

// main window canvas
CDrawPort *pdp;
CDrawPort *pdpNormal;
CDrawPort *pdpWideScreen;
CViewPort *pvpViewPort;

// try to start a new display mode
BOOL TryToSetDisplayMode( enum GfxAPIType eGfxAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                          enum DisplayDepth eColorDepth, BOOL bFullScreenMode)
{
  CDisplayMode dmTmp;
  dmTmp.dm_ddDepth = eColorDepth;
  CPrintF( TRANS("  Starting display mode: %dx%dx%s (%s)\n"),
           pixSizeI, pixSizeJ, (const char *) dmTmp.DepthString(),
           bFullScreenMode ? TRANS("fullscreen") : TRANS("window"));

  // mark to start ignoring window size/position messages until settled down
  _bWindowChanging = TRUE;
  
  // destroy canvas if existing
  //_pGame->DisableLoadingHook();
  if( pvpViewPort!=NULL) {
    _pGfx->DestroyWindowCanvas( pvpViewPort);
    pvpViewPort = NULL;
    pdpNormal = NULL;
  }

  // close the application window
  pMainWin->close();

  pMainWin->setW(pixSizeI);
  pMainWin->setH(pixSizeJ);

  // try to set new display mode
  BOOL bSuccess;
  if( bFullScreenMode) {
#ifdef SE1_D3D
    if( eGfxAPI==GAT_D3D) OpenMainWindowFullScreen( pixSizeI, pixSizeJ);
#endif // SE1_D3D
    pMainWin->setMode(SE_WINDOW_MODE_FULLSCREEN);
    bSuccess = _pGfx->SetDisplayMode( eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth);
    if( bSuccess && eGfxAPI==GAT_OGL) pMainWin->open();
  } else {
#ifdef SE1_D3D
    if( eGfxAPI==GAT_D3D) OpenMainWindowNormal( pixSizeI, pixSizeJ);
#endif // SE1_D3D
    bSuccess = _pGfx->ResetDisplayMode( eGfxAPI);
    if( bSuccess && eGfxAPI==GAT_OGL) pMainWin->open();
#ifdef SE1_D3D
    if( bSuccess && eGfxAPI==GAT_D3D) ResetMainWindowNormal();
#endif // SE1_D3D
  }

  // if new mode was set
  if( bSuccess) {
    // create canvas
    ASSERT( pvpViewPort==NULL);
    ASSERT( pdpNormal==NULL);
    _pGfx->CreateWindowCanvas( pMainWin->getPWindow(), &pvpViewPort, &pdpNormal);

    // erase context of both buffers (for the sake of wide-screen)
    pdp = pdpNormal;
    if( pdp!=NULL && pdp->Lock()) {
      pdp->Fill(C_BLACK|CT_OPAQUE);
      pdp->Unlock();
      pvpViewPort->SwapBuffers();
      pdp->Lock();
      pdp->Fill(C_BLACK|CT_OPAQUE);
      pdp->Unlock();
      pvpViewPort->SwapBuffers();
    }
/*
    // lets try some wide screen screaming :)
    const PIX pixYBegAdj = pdp->GetHeight() * 21/24;
    const PIX pixYEndAdj = pdp->GetHeight() * 3/24;
    const PIX pixXEnd    = pdp->GetWidth();
    pdpWideScreen = new CDrawPort( pdp, PIXaabbox2D( PIX2D(0,pixYBegAdj), PIX2D(pixXEnd, pixYEndAdj)));
    pdpWideScreen->dp_fWideAdjustment = 9.0f / 12.0f;
    if( sam_bWideScreen) pdp = pdpWideScreen;
*/
    // initial screen fill and swap, just to get context running
    BOOL bSuccess = FALSE;
    if( pdp!=NULL && pdp->Lock()) {
      pdp->Fill(pGame->LCDGetColor(C_dGREEN|CT_OPAQUE, "disabled unselected"));
      pdp->Unlock();
      pvpViewPort->SwapBuffers();
      bSuccess = TRUE;
    }/*
    _pGame->EnableLoadingHook(pdp);

    // if the mode is not working, or is not accelerated
    if( !bSuccess || !_pGfx->IsCurrentModeAccelerated())
    { // report error
      CPrintF( TRANS("This mode does not support hardware acceleration.\n"));
      // destroy canvas if existing
      if( pvpViewPort!=NULL) {
        _pGame->DisableLoadingHook();
        _pGfx->DestroyWindowCanvas( pvpViewPort);
        pvpViewPort = NULL;
        pdpNormal = NULL;
      }
      // close the application window
      CloseMainWindow();
      // report failure
      return FALSE;
    }

    // remember new settings
    sam_bFullScreenActive = bFullScreenMode;
    sam_iScreenSizeI = pixSizeI;
    sam_iScreenSizeJ = pixSizeJ;
    sam_iDisplayDepth = eColorDepth;
    sam_iDisplayAdapter = iAdapter;
    sam_iGfxAPI = eGfxAPI;

    // report success
    return TRUE;
  // if couldn't set new mode
  */
  }/* else {
    // close the application window
    CloseMainWindow();
    // report failure
    return FALSE;
  }
  */
}

// start new display mode
void StartNewMode( enum GfxAPIType eGfxAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                   enum DisplayDepth eColorDepth, BOOL bFullScreenMode)
{
  CPrintF( TRANS("\n* START NEW DISPLAY MODE ...\n"));

  // try to set the mode
  BOOL bSuccess = TryToSetDisplayMode( eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth, bFullScreenMode);

  // if failed
  if( !bSuccess)
  {
    // report failure and reset to default resolution
    _iDisplayModeChangeFlag = 2;  // failure
    CPrintF( TRANS("Requested display mode could not be set!\n"));
    pixSizeI = 640;
    pixSizeJ = 480;
    bFullScreenMode = FALSE;
    // try to revert to one of recovery modes
    for( INDEX iMode=0; iMode<ctDefaultModes; iMode++) {
      eColorDepth = (DisplayDepth)aDefaultModes[iMode][0];
      eGfxAPI     = (GfxAPIType)  aDefaultModes[iMode][1];
      iAdapter    =               aDefaultModes[iMode][2];
      CPrintF(TRANSV("\nTrying recovery mode %d...\n"), iMode);
      bSuccess = TryToSetDisplayMode( eGfxAPI, iAdapter, pixSizeI, pixSizeJ, eColorDepth, bFullScreenMode);
      if( bSuccess) break;
    }
    // if all failed
    if( !bSuccess) {
      FatalError(TRANS(
        "Cannot set display mode!\n"
        "Serious Sam was unable to find display mode with hardware acceleration.\n"
        "Make sure you install proper drivers for your video card as recommended\n"
        "in documentation and set your desktop to 16 bit (65536 colors).\n"
        "Please see ReadMe file for troubleshooting information.\n"));
    }

  // if succeeded
  } else {
    _iDisplayModeChangeFlag = 1;  // all ok
  }

  // apply 3D-acc settings
  //ApplyGLSettings(FALSE);

  // remember time of mode setting
  _tmDisplayModeChanged = _pTimer->GetRealTimeTick();
}

BOOL Init(CTString strCmdLine)
{
  scr_splashscreen = new SESplashScreen();
  pMenu = new SEMenu();
  pGame = new SEGame();
  pMainWin = new SEMainWindow();
  pSDL = new SEInterfaceSDL();

  if(!pSDL->init()) {
      FatalError("SDL_Init(VIDEO|AUDIO) failed. Reason: [%s].", pSDL->getError());
      return FALSE;
  }

  scr_splashscreen->setBitmapFile("Splash.bmp");
  scr_splashscreen->show();
  
  // remember desktop width
  _pixDesktopWidth = pSDL->desktopWidth();

  // parse command line before initializing engine
  // FIXME: Maybe add this in future
  // ParseCommandLine(strCmdLine);

  // initialize engine
  SE_InitEngine(argv0, sam_strGameName);


  // init game - this will load persistent symbols
  pGame->init(CTString("Data\\SeriousSam.gms"));

  SE_LoadDefaultFonts();

/*
  // lock the directory
  DirectoryLockOn();

  // load all translation tables
  InitTranslation();
  try {
    AddTranslationTablesDir_t(CTString("Data\\Translations\\"), CTString("*.txt"));
    FinishTranslationTable();
  } catch (char *strError) {
    FatalError("%s", strError);
  }

  // always disable all warnings when in serious sam
  _pShell->Execute( "con_bNoWarnings=1;");

  InitializeGame();
  _pNetwork->md_strGameID = sam_strGameName;

  _pGame->LCDInit();
*/
  if( sam_bFirstStarted) {
    InfoMessage("%s", TRANS(
      "SeriousSam is starting for the first time.\n"
      "If you experience any problems, please consult\n"
      "ReadMe file for troubleshooting information."));
  }

  // initialize sound library
  snd_iFormat = Clamp( snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
  _pSound->SetFormat( (enum CSoundLibrary::SoundFormat)snd_iFormat);
/*
  if (sam_bAutoAdjustAudio) {
    _pShell->Execute("include \"Scripts\\Addons\\SFX-AutoAdjust.ini\"");
  }

  // execute script given on command line
  if (cmd_strScript!="") {
    CPrintF("Command line script: '%s'\n", (const char *) cmd_strScript);
    CTString strCmd;
    strCmd.PrintF("include \"%s\"", (const char *) cmd_strScript);
    _pShell->Execute(strCmd);
  }
  
  // load logo textures
  LoadAndForceTexture(_toLogoCT,   _ptoLogoCT,   CTFILENAME("Textures\\Logo\\LogoCT.tex"));
  LoadAndForceTexture(_toLogoODI,  _ptoLogoODI,  CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
  LoadAndForceTexture(_toLogoEAX,  _ptoLogoEAX,  CTFILENAME("Textures\\Logo\\LogoEAX.tex"));

  LoadStringVar(CTString("Data\\Var\\Sam_Version.var"), sam_strVersion);
  LoadStringVar(CTString("Data\\Var\\ModName.var"), sam_strModName);
  CPrintF(TRANSV("Serious Sam version: %s\n"), (const char *) sam_strVersion);
  CPrintF(TRANSV("Active mod: %s\n"), (const char *) sam_strModName);
  */

  pMenu->init();
  /*
  // if there is a mod
  if (_fnmMod!="") {
    // execute the mod startup script
    _pShell->Execute(CTString("include \"Scripts\\Mod_startup.ini\";"));
  }
*/
  // init gl settings module
  InitGLSettings();
/*
  // init level-info subsystem
  LoadLevelsList();
  LoadDemosList();
*/
  // apply application mode
  StartNewMode( (GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
                (enum DisplayDepth)sam_iDisplayDepth, sam_bFullScreenActive);
/*
  // set default mode reporting
  if( sam_bFirstStarted) {
    _iDisplayModeChangeFlag = 0;
    sam_bFirstStarted = FALSE;
  }
  */
  scr_splashscreen->hide();
/*
  if (cmd_strPassword!="") {
    _pShell->SetString("net_strConnectPassword", cmd_strPassword);
  }

  // if connecting to server from command line
  if (cmd_strServer!="") {
    CTString strPort = "";
    if (cmd_iPort>0) {
      _pShell->SetINDEX("net_iPort", cmd_iPort);
      strPort.PrintF(":%d", cmd_iPort);
    }
    CPrintF(TRANSV("Command line connection: '%s%s'\n"), (const char *) cmd_strServer, (const char *) strPort);
    // go to join menu
    _pGame->gam_strJoinAddress = cmd_strServer;
    if (cmd_bQuickJoin) {
      extern void JoinNetworkGame(void);
      JoinNetworkGame();
    } else {
      StartMenus("join");
    }
  // if starting world from command line
  } else if (cmd_strWorld!="") {
    CPrintF(TRANSV("Command line world: '%s'\n"), (const char *) cmd_strWorld);
    // try to start the game with that level
    try {
      if (cmd_iGoToMarker>=0) {
        CPrintF(TRANSV("Command line marker: %d\n"), cmd_iGoToMarker);
        CTString strCommand;
        strCommand.PrintF("cht_iGoToMarker = %d;", cmd_iGoToMarker);
        _pShell->Execute(strCommand);
      }
      _pGame->gam_strCustomLevel = cmd_strWorld;
      if (cmd_bServer) {
        extern void StartNetworkGame(void);
        StartNetworkGame();
      } else {
        extern void StartSinglePlayerGame(void);
        StartSinglePlayerGame();
      }
    } catch (char *strError) {
      CPrintF(TRANSV("Cannot start '%s': '%s'\n"), (const char *) cmd_strWorld, strError);
    }
  // if no relevant starting at command line
  } else {
    StartNextDemo();
  }
  */
  return TRUE;
}


int SubMain(LPSTR lpCmdLine)
{
  if( !Init(lpCmdLine)) return FALSE;

  // initialy, application is running and active, console and menu are off
  _bRunning    = TRUE;
  _bQuitScreen = TRUE;
  /*
 _pGame->gm_csConsoleState  = CS_OFF;
  _pGame->gm_csComputerState = CS_OFF;
//  bMenuActive    = FALSE;
//  bMenuRendering = FALSE;
  // while it is still running
  */
  while( _bRunning)
  {
      
    // while there are any messages in the message queue
    MSG msg;
    
    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) {
      // if it is not a mouse message
      if( !(msg.message>=WM_MOUSEFIRST && msg.message<=WM_MOUSELAST) ) {
        // if not system key messages
        if( !((msg.message==WM_KEYDOWN && msg.wParam==VK_F10)
            ||msg.message==WM_SYSKEYDOWN)) {
          // dispatch it
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
/*
      // toggle full-screen on alt-enter
      if( msg.message==WM_SYSKEYDOWN && msg.wParam==VK_RETURN && !IsIconic(_hwndMain)) {
        // !!! FIXME: SDL doesn't need to rebuild the GL context here to toggle fullscreen.
        STUBBED("SDL doesn't need to rebuild the GL context here...");
        StartNewMode( (GfxAPIType)sam_iGfxAPI, sam_iDisplayAdapter, sam_iScreenSizeI, sam_iScreenSizeJ,
                      (enum DisplayDepth)sam_iDisplayDepth, !sam_bFullScreenActive);

        if (_pInput != NULL) // rcg02042003 hack for SDL vs. Win32.
          _pInput->ClearRelativeMouseMotion();
      }
*/
      // if application should stop
      if( msg.message==WM_QUIT || msg.message==WM_CLOSE) {
        // stop running
        _bRunning = FALSE;
        _bQuitScreen = FALSE;
      }
/*
      if (msg.message==WM_KEYDOWN && msg.wParam==VK_ESCAPE && 
        (_gmRunningGameMode==GM_DEMO || _gmRunningGameMode==GM_INTRO)) {
        _pGame->StopGame();
        _gmRunningGameMode=GM_NONE;
      }

      if (_pGame->gm_csConsoleState==CS_TALK && msg.message==WM_KEYDOWN && msg.wParam==VK_ESCAPE) {
        if (_pInput != NULL) // rcg02042003 hack for SDL vs. Win32.
          _pInput->ClearRelativeMouseMotion();
        _pGame->gm_csConsoleState = CS_OFF;
        msg.message=WM_NULL;
      }

      BOOL bMenuForced = (_gmRunningGameMode==GM_NONE && 
        (_pGame->gm_csConsoleState==CS_OFF || _pGame->gm_csConsoleState==CS_TURNINGOFF));
      BOOL bMenuToggle = (msg.message==WM_KEYDOWN && msg.wParam==VK_ESCAPE 
        && (_pGame->gm_csComputerState==CS_OFF || _pGame->gm_csComputerState==CS_ONINBACKGROUND));
      if( !bMenuActive) {
        if( bMenuForced || bMenuToggle) {
          // if console is active
          if( _pGame->gm_csConsoleState==CS_ON || _pGame->gm_csConsoleState==CS_TURNINGON) {
            // deactivate it
            _pGame->gm_csConsoleState = CS_TURNINGOFF;
            _iAddonExecState = 0;
          }
          // delete key down message so menu would not exit because of it
          msg.message=WM_NULL;
          // start menu
          StartMenus();
        }
      } else {
        if (bMenuForced && bMenuToggle && pgmCurrentMenu->gm_pgmParentMenu == NULL) {
          // delete key down message so menu would not exit because of it
          msg.message=WM_NULL;
        }
      }

      // if neither menu nor console is running
      if (!bMenuActive && (_pGame->gm_csConsoleState==CS_OFF || _pGame->gm_csConsoleState==CS_TURNINGOFF)) {
        // if current menu is not root
        if (!IsMenusInRoot()) {
          // start current menu
          StartMenus();
        }
      }

      if (sam_bMenuSave) {
        sam_bMenuSave = FALSE;
        StartMenus("save");
      }
      if (sam_bMenuLoad) {
        sam_bMenuLoad = FALSE;
        StartMenus("load");
      }
      if (sam_bMenuControls) {
        sam_bMenuControls = FALSE;
        StartMenus("controls");
      }
      if (sam_bMenuHiScore) {
        sam_bMenuHiScore = FALSE;
        StartMenus("hiscore");
      }

      // interpret console key presses
      if (_iAddonExecState==0) {
        if (msg.message==WM_KEYDOWN) {
          _pGame->ConsoleKeyDown(msg);
          if (_pGame->gm_csConsoleState!=CS_ON) {
            _pGame->ComputerKeyDown(msg);
          }
        } else if (msg.message==WM_KEYUP) {
          // special handler for talk (not to invoke return key bind)
          if( msg.wParam==VK_RETURN && _pGame->gm_csConsoleState==CS_TALK)
          {
            if (_pInput != NULL) // rcg02042003 hack for SDL vs. Win32.
              _pInput->ClearRelativeMouseMotion();
            _pGame->gm_csConsoleState = CS_OFF;
          }
        } else if (msg.message==WM_CHAR) {
          _pGame->ConsoleChar(msg);
        }
        if (msg.message==WM_LBUTTONDOWN
          ||msg.message==WM_RBUTTONDOWN
          ||msg.message==WM_LBUTTONDBLCLK
          ||msg.message==WM_RBUTTONDBLCLK
          ||msg.message==WM_LBUTTONUP
          ||msg.message==WM_RBUTTONUP) {
          if (_pGame->gm_csConsoleState!=CS_ON) {
            _pGame->ComputerKeyDown(msg);
          }
        }
      }
      // if menu is active and no input on
      if( bMenuActive && !_pInput->IsInputEnabled()) {
        // pass keyboard/mouse messages to menu
        if(msg.message==WM_KEYDOWN) {
          MenuOnKeyDown( msg.wParam);
        } else if (msg.message==WM_LBUTTONDOWN || msg.message==WM_LBUTTONDBLCLK) {
          MenuOnKeyDown(VK_LBUTTON);
        } else if (msg.message==WM_RBUTTONDOWN || msg.message==WM_RBUTTONDBLCLK) {
          MenuOnKeyDown(VK_RBUTTON);
        } else if (msg.message==WM_MOUSEMOVE) {
          MenuOnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
#ifndef WM_MOUSEWHEEL
 #define WM_MOUSEWHEEL 0x020A
#endif
        } else if (msg.message==WM_MOUSEWHEEL) {
          SWORD swDir = SWORD(UWORD(HIWORD(msg.wParam)));
          if (swDir>0) {
            MenuOnKeyDown(11);
          } else if (swDir<0) {
            MenuOnKeyDown(10);
          }
        } else if (msg.message==WM_CHAR) {
          MenuOnChar(msg);
        }
      }

      // if toggling console
      BOOL bConsoleKey = sam_bToggleConsole || msg.message==WM_KEYDOWN && 
            // !!! FIXME: rcg11162001 This sucks.
            // FIXME: DG: we could use SDL_SCANCODE_GRAVE ?
        #ifdef PLATFORM_UNIX
        (msg.wParam == SDLK_BACKQUOTE
        #else
        (MapVirtualKey(msg.wParam, 0)==41 // scan code for '~'
        #endif
        || msg.wParam==VK_F1 || (msg.wParam==VK_ESCAPE && _iAddonExecState==3));
      if(bConsoleKey && !_bDefiningKey)
      {
        sam_bToggleConsole = FALSE;
        if( _iAddonExecState==3) _iAddonExecState = 0;
        // if it is up, or pulling up
        if( _pGame->gm_csConsoleState==CS_OFF || _pGame->gm_csConsoleState==CS_TURNINGOFF) {
          // start it moving down and disable menu
          _pGame->gm_csConsoleState = CS_TURNINGON;
          // stop all IFeel effects
          IFeel_StopEffect(NULL);
          if( bMenuActive) {
            StopMenus(FALSE);
          }
        // if it is down, or dropping down
        } else if( _pGame->gm_csConsoleState==CS_ON || _pGame->gm_csConsoleState==CS_TURNINGON) {
          // start it moving up
          _pGame->gm_csConsoleState = CS_TURNINGOFF;
        }
      }

      if (_pShell->GetINDEX("con_bTalk") && _pGame->gm_csConsoleState==CS_OFF) {
        _pShell->SetINDEX("con_bTalk", FALSE);
        _pGame->gm_csConsoleState = CS_TALK;
      }

      // if pause pressed
      if (msg.message==WM_KEYDOWN && msg.wParam==VK_PAUSE) {
        // toggle pause
        _pNetwork->TogglePause();
      }

#ifdef PLATFORM_WIN32
      // if command sent from external application
      if (msg.message==WM_COMMAND) {
        // if teleport player
        if (msg.wParam==1001) {
          // teleport player
          TeleportPlayer(msg.lParam);
          // restore
          PostMessage(NULL, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
      }
#endif

      // if demo is playing
      if (_gmRunningGameMode==GM_DEMO ||
          _gmRunningGameMode==GM_INTRO ) {
        // check if escape is pressed
        BOOL bEscape = (msg.message==WM_KEYDOWN && msg.wParam==VK_ESCAPE);
        // check if console-invoke key is pressed
        BOOL bTilde = (msg.message==WM_KEYDOWN && 
          (msg.wParam==VK_F1 ||
            // !!! FIXME: ugly.
            #ifdef PLATFORM_UNIX
              msg.wParam == SDLK_BACKQUOTE
            #else
              MapVirtualKey(msg.wParam, 0)==41 // scan code for '~'
            #endif
          ));
        // check if any key is pressed
        BOOL bAnyKey = (
          (msg.message==WM_KEYDOWN && (msg.wParam==VK_SPACE || msg.wParam==VK_RETURN))|| 
          msg.message==WM_LBUTTONDOWN||msg.message==WM_RBUTTONDOWN);

        // if escape is pressed
        if (bEscape) {
          // stop demo
          _pGame->StopGame();
          _bInAutoPlayLoop = FALSE;
          _gmRunningGameMode = GM_NONE;
        // if any other key is pressed except console invoking
        } else if (bAnyKey && !bTilde) {
          // if not in menu or in console
          if (!bMenuActive && !bMenuRendering && _pGame->gm_csConsoleState==CS_OFF) {
            // skip to next demo
            _pGame->StopGame();
            _gmRunningGameMode = GM_NONE;
            StartNextDemo();        
          }
        }
      }
*/
    } // loop while there are messages
/*
    // when all messages are removed, window has surely changed
    _bWindowChanging = FALSE;

    // get real cursor position
    if( _pGame->gm_csComputerState!=CS_OFF && _pGame->gm_csComputerState!=CS_ONINBACKGROUND) {
      POINT pt;
      ::GetCursorPos(&pt);
      ::ScreenToClient(_hwndMain, &pt);
      _pGame->ComputerMouseMove(pt.x, pt.y);
    }

    // if addon is to be executed
    if (_iAddonExecState==1) {
      // print header and start console
      CPrintF(TRANSV("---- Executing addon: '%s'\n"), (const char*)_fnmAddonToExec);
      sam_bToggleConsole = TRUE;
      _iAddonExecState = 2;
    // if addon is ready for execution
    } else if (_iAddonExecState==2 && _pGame->gm_csConsoleState == CS_ON) {
      // execute it
      CTString strCmd;
      strCmd.PrintF("include \"%s\"", (const char*)_fnmAddonToExec);
      _pShell->Execute(strCmd);
      CPrintF(TRANSV("Addon done, press Escape to close console\n"));
      _iAddonExecState = 3;
    }

    // automaticaly manage input enable/disable toggling
    UpdateInputEnabledState();
    // automaticaly manage pause toggling
    UpdatePauseState();
    // notify game whether menu is active
    _pGame->gm_bMenuOn = bMenuActive;

    // do the main game loop and render screen
    DoGame();

    // limit current frame rate if neeeded
    LimitFrameRate();
    */

  } // end of main application loop
/*
  _pInput->DisableInput();
  _pGame->StopGame();
  
  // invoke quit screen if needed
  if( _bQuitScreen && _fnmModToLoad=="") QuitScreenLoop();
  
  End();
  */
  return TRUE;
}


int main(int argc, char **argv)
{
  argv0 = argv[0];

  CTString cmdLine;
  for (int i = 1; i < argc; i++) {
    cmdLine += " \"";
    cmdLine += argv[i];
    cmdLine += "\"";
  }
  
  int iResult;
  CTSTREAM_BEGIN {
    iResult = SubMain((char *) ((const char *) cmdLine));
  } CTSTREAM_END;
  /*
  CheckModReload();

  CheckTeaser();

  CheckBrowser();
*/
  return iResult;
}