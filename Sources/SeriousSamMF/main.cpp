#include <SDL.h>
#include <SDL2/SDL_keycode.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/Stream.h>
#include <Engine/Engine.h>
#include <Engine/Sound/SoundLibrary.h>
#include <Engine/Base/SDL/SDLEvents.h>

#include "SplashScreen.h"
#include "Menu.h"
#include "MainWindow.h"
#include "InterfaceSDL.h"
#include "Colors.h"

#define SSMF_WINDOW_RECOVERY_W 640
#define SSMF_WINDOW_RECOVERY_H 480

static char *argv0 = NULL;

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
ENGINE_API extern INDEX snd_iFormat;

BOOL _bRunning = FALSE;
BOOL _bQuitScreen = FALSE;
BOOL sam_bFirstStarted = TRUE; /* FIXME: Check if is first start */

PIX  _pixDesktopWidth = 0;    // desktop width when started (for some tests)

CTString sam_strGameName = "serioussammf";

SESplashScreen* scr_splashscreen = NULL;
SEMenu* pMenu = NULL;
CMainMenu* pMainMenu = NULL;
SEMainWindow* pMainWin = NULL;
SERender* pRender = NULL;

POINT pt;
SDL_Event* event = NULL;
// display mode settings
INDEX iWindowMode = SE_WINDOW_MODE_WINDOWED;
INDEX iWindowAPI = 0;         // 0==OpenGL
INDEX iWindowW = 640;  // current size of the window
INDEX iWindowH = 480;  // current size of the window
INDEX iWindowDepth  = 0;  // 0==default, 1==16bit, 2==32bit
INDEX iWindowAdapter = 0; 

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

/*
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
  
  _pShell->DeclareSymbol("persistent CTString sam_strLastRenderer;", (void *) &_strLastRenderer);
  _pShell->DeclareSymbol("persistent INDEX    sam_iLastSetup;",      (void *) &_iLastPreferences);
  
}
*/
/*
// start new display mode
void StartNewMode( )
{
  CPrintF( TRANS("\n* START NEW DISPLAY MODE ...\n"));

  // try to set the mode
  BOOL bSuccess = TryToSetDisplayMode();
/*
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
*/
  // apply 3D-acc settings
  //ApplyGLSettings(FALSE);

/*
}
*/

static FLOAT bMenuRendering = 0.25f;

BOOL canChangeResolution(PIX w, PIX h)
{
    if(pMainWin->getW() != w || pMainWin->getH() != h) {
        pRender->destroy();
        pMainWin->setW(w);
        pMainWin->setH(h);
        pMainWin->create();
        pRender->create(pMainWin->getPWindow());
    }
    return TRUE;
}

extern int SE_SDL_InputEventPoll(SDL_Event *event);

void update()
{
    // get real cursor position
    GetCursorPos(&pt);
    while (SE_SDL_InputEventPoll(event)) {
        if(event->type == SDL_QUIT) {
            _bRunning = FALSE;
        }
        if(event->type == SDL_KEYDOWN) {
            INDEX ksym = event->key.keysym.sym;
            switch(ksym) {

            case SDLK_ESCAPE:
                _bRunning = FALSE;
                break;
            case SDLK_F1:
                canChangeResolution(640, 480); //VGA 4:3 
                break;
            case SDLK_F2:
                canChangeResolution(800, 600); //SVGA 4:3
                break;
            case SDLK_F3:
                canChangeResolution(640, 360); //16:9
                break;
            case SDLK_F4:
                canChangeResolution(1280, 720); //16:9
                break;
            }
        }
    }
}

void GameLoop()
{
  #ifdef SINGLE_THREADED
    _pTimer->HandleTimerHandlers();
  #endif
  if( !pMainWin->isIconic() ) 
  {
    update();
    pMainMenu->update(event, pt);

    if(pRender->lock()) {
      pRender->fill(SE_COL_ORANGE_NEUTRAL|255);
      // do menu
      if( bMenuRendering) {
        // clear z-buffer
        pRender->fillZBuffer(ZBUF_BACK);
        // remember if we should render menus next tick
        pMenu->render(pRender);
        pMainMenu->render(pRender);
        
      }
      // done with all
      pRender->unlock();
      pRender->SwapBuffers();
    }
  }
}

BOOL Init(CTString strCmdLine)
{
  scr_splashscreen = new SESplashScreen();
  pMainWin = new SEMainWindow();
  pRender = new SERender();
  event = new SDL_Event();

  if(!SEInterfaceSDL::init()) {
      FatalError("SDL_Init(VIDEO|AUDIO) failed. Reason: [%s].", SEInterfaceSDL::getError());
      return FALSE;
  }

  scr_splashscreen->setBitmapFile("Splash.bmp");
  scr_splashscreen->show();
  
  // remember desktop width
  _pixDesktopWidth = SEInterfaceSDL::desktopWidth();

  // parse command line before initializing engine
  // FIXME: Maybe add this in future
  // ParseCommandLine(strCmdLine);

  // initialize engine
  SE_InitEngine(argv0, sam_strGameName);

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
  /*if( sam_bFirstStarted) {
    InfoMessage("%s", TRANS(
      "SeriousSam is starting for the first time.\n"
      "If you experience any problems, please consult\n"
      "ReadMe file for troubleshooting information."));
  }*/

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
  */

/*
  LoadStringVar(CTString("Data\\Var\\Sam_Version.var"), sam_strVersion);
  LoadStringVar(CTString("Data\\Var\\ModName.var"), sam_strModName);
  CPrintF(TRANSV("Serious Sam version: %s\n"), (const char *) sam_strVersion);
  CPrintF(TRANSV("Active mod: %s\n"), (const char *) sam_strModName);
  */
  /*
  // if there is a mod
  if (_fnmMod!="") {
    // execute the mod startup script
    _pShell->Execute(CTString("include \"Scripts\\Mod_startup.ini\";"));
  }
*/
  // init gl settings module
  //InitGLSettings();
/*
  // init level-info subsystem
  LoadLevelsList();
  LoadDemosList();
*/
  // apply application mode
  pMainWin->setTitle(sam_strGameName);
  pMainWin->setW(iWindowW);
  pMainWin->setH(iWindowH);
  pMainWin->setDepth(DisplayDepth::DD_DEFAULT);
  pMainWin->setAdapter(iWindowAdapter);
  BOOL winResult = pMainWin->create();
  pRender->create(pMainWin->getPWindow());

  if(! winResult) {
    pMainWin->setW(SSMF_WINDOW_RECOVERY_W);
    pMainWin->setH(SSMF_WINDOW_RECOVERY_H);
    for(int i = 0; i < ctDefaultModes; i++) {
      pMainWin->setDepth((DisplayDepth) aDefaultModes[i][0]);
      pMainWin->setAPI( (GfxAPIType)  aDefaultModes[i][1]);
      pMainWin->setAdapter(aDefaultModes[i][2]);
      CPrintF(TRANSV("\nTrying recovery mode %d...\n"), i);
      winResult = pMainWin->create();
      if( winResult ) break;
    }
  }
    // if all failed
    if( !winResult) {
      FatalError(TRANS(
        "Cannot set display mode!\n"
        "Serious Sam was unable to find display mode with hardware acceleration.\n"
        "Make sure you install proper drivers for your video card as recommended\n"
        "in documentation and set your desktop to 16 bit (65536 colors).\n"
        "Please see ReadMe file for troubleshooting information.\n"));
    }

  pRender->setVirtX(640);
  pRender->setVirtY(480);

  // remember time of mode setting
  _tmDisplayModeChanged = _pTimer->GetRealTimeTick();
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

  pMenu = new SEMenu();
  pMainMenu = new CMainMenu();

  pMenu->setActive(TRUE);
  pMenu->  _ptoLogoCT  = pRender->loadTexture(CTFILENAME("Textures\\Logo\\LogoCT.tex"));
  pMenu->  _ptoLogoODI = pRender->loadTexture(CTFILENAME("Textures\\Logo\\GodGamesLogo.tex"));
  pMenu->  _ptoLogoEAX = pRender->loadTexture(CTFILENAME("Textures\\Logo\\LogoEAX.tex"));
  pMenu->init();
  // initialy, application is running and active, console and menu are off
  _bRunning    = TRUE;
  _bQuitScreen = TRUE;

  while(_bRunning)
  {
    GameLoop();
  }
  /*
 _pGame->gm_csConsoleState  = CS_OFF;
  _pGame->gm_csComputerState = CS_OFF;
//  bMenuActive    = FALSE;
//  bMenuRendering = FALSE;
  // while it is still running
  */
  
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
*/
    // do the main game loop and render screen
/*
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