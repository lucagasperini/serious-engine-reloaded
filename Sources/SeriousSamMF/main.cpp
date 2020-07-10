#include <SDL.h>
#include <SDL2/SDL_keycode.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/Stream.h>
#include <Engine/Engine.h>
#include <Engine/Sound/SoundLibrary.h>
#include <Engine/Base/SDL/SDLEvents.h>

#include "SplashScreen.h"
#include "MainWindow.h"
#include "InterfaceSDL.h"
#include "Colors.h"
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"
#include "ECS/Manager.h"
#include "ECS/RenderSystem.h"
#include "ECS/PositionSystem.h"
#include "ECS/InputSystem.h"

#include <inttypes.h>

#define SSMF_WINDOW_RECOVERY_W 640
#define SSMF_WINDOW_RECOVERY_H 480

static char *argv0 = NULL;

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
ENGINE_API extern INDEX snd_iFormat;

BOOL _bQuitScreen = FALSE;
BOOL sam_bFirstStarted = TRUE; /* FIXME: Check if is first start */

PIX  _pixDesktopWidth = 0;    // desktop width when started (for some tests)

CTString sam_strGameName = "serioussammf";

SESplashScreen* scr_splashscreen = NULL;
SEMainWindow* pMainWin = NULL;
CDrawPort* main_dp = NULL;
CViewPort* main_vp = NULL;
ECSManager* manager = NULL;

PositionSystem* position_system = NULL;
RenderSystem* render_system = NULL;
InputSystem* input_system = NULL;

CFontData* main_fb_font;
COLOR main_fb_color = C_BLACK|0xff;

POINT cursor;
SDL_Event* event = NULL;
// display mode settings
INDEX iWindowMode = SE_WINDOW_MODE_WINDOWED;
INDEX iWindowAPI = 0;         // 0==OpenGL
INDEX iWindowW = 1280;  // current size of the window
INDEX iWindowH = 720;  // current size of the window
INDEX iWindowDepth  = 0;  // 0==default, 1==16bit, 2==32bit
INDEX iWindowAdapter = 0; 

// 0...app started for the first time
// 1...all ok
// 2...automatic fallback
static INDEX _iDisplayModeChangeFlag = 0;
static TIME _tmDisplayModeChanged = 100.0f; // when display mode was last changed

CPlayerEntity *pen = NULL;

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

BOOL runningMenu;
BOOL runningGame;
BOOL runningLevel;

void quitgame()
{
    runningGame = FALSE;
}

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

BOOL canChangeResolution(PIX w, PIX h, BOOL fullscreen)
{
    if(pMainWin->getW() != w || pMainWin->getH() != h) {
        if(fullscreen)
            pMainWin->setMode(SE_WINDOW_MODE_FULLSCREEN);
        else
            pMainWin->setMode(SE_WINDOW_MODE_WINDOWED);
        pMainWin->setW(w);
        pMainWin->setH(h);
        pMainWin->create();
        main_dp = pMainWin->getDrawPort();
        main_vp = pMainWin->getViewPort();
        position_system->init();
    }
    return TRUE;
}

extern int SE_SDL_InputEventPoll(SDL_Event *event);
FLOAT sensibility = 0.5;
POINT* old_point = NULL;
void update()
{
    // get real cursor position
    GetCursorPos(&cursor);
    if(runningLevel) {
      if(old_point == NULL) {
        old_point = new POINT(cursor);
      } else {
        POINT delta;
        delta.x = (old_point->x - cursor.x) * sensibility;
        delta.y = (old_point->y - cursor.y) * sensibility;
        pen->en_plPlacement.Rotate_Airplane(ANGLE3D(delta.x, delta.y, 0.0f));
        old_point->x = cursor.x;
        old_point->y = cursor.y;
      }
    }
    while (SE_SDL_InputEventPoll(event)) {
        if(event->type == SDL_QUIT) {
            quitgame();
        }
        if(event->type == SDL_KEYDOWN) {
            INDEX ksym = event->key.keysym.sym;
            switch(ksym) {

            case SDLK_ESCAPE:
                quitgame();
                break;
            case SDLK_F1:
                canChangeResolution(640, 480, FALSE); //VGA 4:3 
                break;
            case SDLK_F2:
                canChangeResolution(800, 600, FALSE); //SVGA 4:3
                break;
            case SDLK_F3:
                canChangeResolution(1280, 720, FALSE); //16:9
                break;
            case SDLK_F4:
                canChangeResolution(1920, 1080, TRUE); //16:9
                break;
            case SDLK_F5:
                render_system->dbg_draw_border=!render_system->dbg_draw_border;
                break;
            case SDLK_F6:
                render_system->dbg_draw_id=!render_system->dbg_draw_id;
                break;
            case SDLK_F7:
                render_system->dbg_draw_position=!render_system->dbg_draw_position;
                break;
            case SDLK_F8:
                pen->en_plPlacement = CPlacement3D(FLOAT3D(10,40,0),ANGLE3D(90,0,10));
                break;
            case SDLK_RIGHT:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(10.0f,0.0f,0.0f));
                break;
            case SDLK_LEFT:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(-10.0f,0.0f,0.0f));
                break;
            case SDLK_UP:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,10.0f,0.0f));
                break;
            case SDLK_DOWN:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,-10.0f,0.0f));
                break;
            case SDLK_SPACE:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,0.0f,10.0f));
                break;
            case SDLK_c:
                pen->en_plPlacement.Translate_AbsoluteSystem(FLOAT3D(0.0f,0.0f,-10.0f));
                break;
            }
        }
    }
}

BOOL Init(CTString strCmdLine)
{
  scr_splashscreen = new SESplashScreen();
  pMainWin = new SEMainWindow();
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
  main_dp = pMainWin->getDrawPort();
  main_vp = pMainWin->getViewPort();

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

/*
CImageInfo iiImageInfo;
   iiImageInfo.LoadAnyGfxFormat_t( fntex);
    // both dimension must be potentions of 2
    if( (iiImageInfo.ii_Width  == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Width))) &&
        (iiImageInfo.ii_Height == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Height))) )
    {
      CTFileName fnTexture = fntex.FileDir()+fntex.FileName()+".tex";
      // creates new texture with one frame
      CTextureData tdPicture;
      tdPicture.Create_t( &iiImageInfo, iiImageInfo.ii_Width, 1, FALSE);
      tdPicture.Save_t( fnTexture);
    }
*/
class CSessionProperties {
public:
  enum GameMode {
    GM_FLYOVER = -1,
    GM_COOPERATIVE = 0,
    GM_SCOREMATCH,
    GM_FRAGMATCH,
  };
  enum GameDifficulty {
    GD_TOURIST = -1,
    GD_EASY = 0,
    GD_NORMAL,
    GD_HARD,
    GD_EXTREME,
  };

  INDEX sp_ctMaxPlayers;    // maximum number of players in game
  BOOL sp_bWaitAllPlayers;  // wait for all players to connect
  BOOL sp_bQuickTest;       // set when game is tested from wed
  BOOL sp_bCooperative;     // players are not intended to kill each other
  BOOL sp_bSinglePlayer;    // single player mode has some special rules
  BOOL sp_bUseFrags;        // set if frags matter instead of score

  enum GameMode sp_gmGameMode;    // general game rules

  enum GameDifficulty sp_gdGameDifficulty;
  ULONG sp_ulSpawnFlags;
  BOOL sp_bMental;            // set if mental mode engaged

  INDEX sp_iScoreLimit;       // stop game after a player/team reaches given score
  INDEX sp_iFragLimit;        // stop game after a player/team reaches given score
  INDEX sp_iTimeLimit;        // stop game after given number of minutes elapses

  BOOL sp_bTeamPlay;          // players are divided in teams
  BOOL sp_bFriendlyFire;      // can harm player of same team
  BOOL sp_bWeaponsStay;       // weapon items do not dissapear when picked-up
  BOOL sp_bAmmoStays;         // ammo items do not dissapear when picked-up
  BOOL sp_bHealthArmorStays;  // health/armor items do exist
  BOOL sp_bPlayEntireGame;    // don't finish after one level in coop
  BOOL sp_bAllowHealth;       // health items do exist
  BOOL sp_bAllowArmor;        // armor items do exist
  BOOL sp_bInfiniteAmmo;      // ammo is not consumed when firing
  BOOL sp_bRespawnInPlace;    // players respawn on the place where they were killed, not on markers (coop only)

  FLOAT sp_fEnemyMovementSpeed; // enemy speed multiplier
  FLOAT sp_fEnemyAttackSpeed;   // enemy speed multiplier
  FLOAT sp_fDamageStrength;     // multiplier when damaged
  FLOAT sp_fAmmoQuantity;       // multiplier when picking up ammo
  FLOAT sp_fManaTransferFactor; // multiplier for the killed player mana that is to be added to killer's mana
  INDEX sp_iInitialMana;        // life price (mana that each player'll have upon respawning)
  FLOAT sp_fExtraEnemyStrength;            // fixed adder for extra enemy power 
  FLOAT sp_fExtraEnemyStrengthPerPlayer;   // adder for extra enemy power per each player playing

  INDEX sp_ctCredits;           // number of credits for this game
  INDEX sp_ctCreditsLeft;       // number of credits left on this level
  FLOAT sp_tmSpawnInvulnerability;   // how many seconds players are invunerable after respawning

  INDEX sp_iBlood;         // blood/gibs type (0=none, 1=green, 2=red, 3=hippie)
  BOOL  sp_bGibs;          // enable/disable gibbing

  BOOL  sp_bEndOfGame;     // marked when dm game is finished (any of the limits reached)

  ULONG sp_ulLevelsMask;    // mask of visited levels so far

  BOOL  sp_bUseExtraEnemies;  // spawn extra multiplayer enemies
};

class CUniversalSessionProperties {
public:
  union {
    CSessionProperties usp_sp;
    UBYTE usp_aubDummy[NET_MAXSESSIONPROPERTIES];
  };

  // must have exact the size as allocated block in engine
  CUniversalSessionProperties() { 
    ASSERT(sizeof(CSessionProperties)<=NET_MAXSESSIONPROPERTIES); 
    ASSERT(sizeof(CUniversalSessionProperties)==NET_MAXSESSIONPROPERTIES); 
    memset(usp_aubDummy, '\0', sizeof (usp_aubDummy));
  }
  operator CSessionProperties&(void) { return usp_sp; }
};

void SetDifficultyParameters(CSessionProperties &sp)
{
  INDEX iDifficulty = 1;
  sp.sp_gdGameDifficulty = (CSessionProperties::GameDifficulty) Clamp(INDEX(iDifficulty), -1, 3);

  sp.sp_ulSpawnFlags = SPF_EASY;//SPF_TOURIST; !!!!
  sp.sp_fEnemyMovementSpeed = 1.0f;
  sp.sp_fEnemyAttackSpeed   = 1.0f;
  sp.sp_fDamageStrength     = 1.0f;
  sp.sp_fAmmoQuantity       = 1.0f;
}

INDEX gam_iStartMode = 0;
INDEX gam_iBlood = 0;
INDEX gam_bGibs = FALSE;
// set properties for a single player session
void SetSinglePlayerSession(CSessionProperties &sp)
{
  // clear
  memset(&sp, 0, sizeof(sp));

  SetDifficultyParameters(sp);
  sp.sp_gmGameMode = (CSessionProperties::GameMode) Clamp(INDEX(gam_iStartMode), -1, 2); //
  sp.sp_ulSpawnFlags&=~SPF_COOPERATIVE;

  sp.sp_bEndOfGame = FALSE;

  sp.sp_ctMaxPlayers = 1;
  sp.sp_bWaitAllPlayers = FALSE;
  sp.sp_bQuickTest = FALSE;
  sp.sp_bCooperative = TRUE;
  sp.sp_bSinglePlayer = TRUE;
  sp.sp_bUseFrags = FALSE;

  sp.sp_iScoreLimit = 0;
  sp.sp_iFragLimit  = 0; 
  sp.sp_iTimeLimit  = 0; 

  sp.sp_ctCredits     = 0;
  sp.sp_ctCreditsLeft = 0;
  sp.sp_tmSpawnInvulnerability = 0;

  sp.sp_bTeamPlay = FALSE;
  sp.sp_bFriendlyFire = FALSE;
  sp.sp_bWeaponsStay = FALSE;
  sp.sp_bPlayEntireGame = TRUE;

  sp.sp_bAmmoStays        = FALSE;
  sp.sp_bHealthArmorStays = FALSE;
  sp.sp_bAllowHealth = TRUE;
  sp.sp_bAllowArmor = TRUE;
  sp.sp_bInfiniteAmmo = FALSE;
  sp.sp_bRespawnInPlace = FALSE;
  sp.sp_fExtraEnemyStrength          = 0;
  sp.sp_fExtraEnemyStrengthPerPlayer = 0;

  sp.sp_iBlood = Clamp( gam_iBlood, 0, 3);
  sp.sp_bGibs  = gam_bGibs;
}

void startGame()
{
  CTString strSessionName = "sessione";
  CTFileName fnmWorld = CTFILENAME("Levels\\TestGame.wld");

  CUniversalSessionProperties sp;
  SetSinglePlayerSession(sp);
  _pNetwork->StartPeerToPeer_t( strSessionName, fnmWorld, 
        0, 1, FALSE, &sp);

  pen = new CPlayerEntity();
  pen->en_pwoWorld = &_pNetwork->ga_World;
  pen->en_plPlacement = CPlacement3D(FLOAT3D(370,40,0),ANGLE3D(-80,20,0));

  runningLevel = TRUE;
}

BOOL _bCameraOn=TRUE;
BOOL cam_bRecord = TRUE;
BOOL _bInitialized = FALSE; 
BOOL cam_bResetToPlayer = TRUE;
INDEX cam_bZoomDefault = TRUE;
class CCameraPos {
public:
  TIME cp_tmTick;
  FLOAT cp_fSpeed;
  FLOAT3D cp_vPos;
  ANGLE3D cp_aRot;
  ANGLE cp_aFOV;
};

//BOOL _bCameraOn=FALSE;
CTFileStream _strScript;
//BOOL _bInitialized;
FLOAT _fStartTime;
CCameraPos _cp0;
CCameraPos _cp1;
CCameraPos _cp;

// camera control
//INDEX cam_bRecord           = FALSE;
static INDEX cam_bMoveForward      = FALSE;
static INDEX cam_bMoveBackward     = FALSE;
static INDEX cam_bMoveLeft         = FALSE;
static INDEX cam_bMoveRight        = FALSE;
static INDEX cam_bMoveUp           = FALSE;
static INDEX cam_bMoveDown         = FALSE;
static INDEX cam_bTurnBankingLeft  = FALSE;
static INDEX cam_bTurnBankingRight = FALSE;
static INDEX cam_bZoomIn           = FALSE;
static INDEX cam_bZoomOut          = FALSE;
//static INDEX cam_bZoomDefault      = FALSE;
//static INDEX cam_bResetToPlayer    = FALSE;
static INDEX cam_bSnapshot         = FALSE;
static INDEX cam_fSpeed            = 1;


void ReadPos(CCameraPos &cp)
{
  try {
    CTString strLine;
    _strScript.GetLine_t(strLine);
    strLine.ScanF("%g: %g: %g %g %g:%g %g %g:%g", 
      &cp.cp_tmTick,
      &cp.cp_fSpeed,
      &cp.cp_vPos(1), &cp.cp_vPos(2), &cp.cp_vPos(3),
      &cp.cp_aRot(1), &cp.cp_aRot(2), &cp.cp_aRot(3),
      &cp.cp_aFOV);

  } catch (char *strError) {
    CPrintF("Camera: %s\n", strError);
  }
}
void WritePos(CCameraPos &cp)
{
  try {
    CTString strLine;
    strLine.PrintF("%g: %g: %g %g %g:%g %g %g:%g", 
      _pTimer->GetLerpedCurrentTick()-_fStartTime,
      1.0f,
      cp.cp_vPos(1), cp.cp_vPos(2), cp.cp_vPos(3),
      cp.cp_aRot(1), cp.cp_aRot(2), cp.cp_aRot(3),
      cp.cp_aFOV);
    _strScript.PutLine_t(strLine);

  } catch (char *strError) {
    CPrintF("Camera: %s\n", strError);
  }
}
void SetSpeed(FLOAT fSpeed)
{
  CTString str;
  str.PrintF("dem_fRealTimeFactor = %g;", fSpeed);
  _pShell->Execute(str);
}


void CAM_Render(CEntity *pen, CDrawPort *pdp)
{
  if( cam_bRecord) {
    if (!_bInitialized) {
      _bInitialized = TRUE;
      SetSpeed(1.0f);
      _fStartTime = _pTimer->CurrentTick();
    }
    FLOATmatrix3D m;
    MakeRotationMatrixFast(m, _cp.cp_aRot);
    FLOAT3D vX, vY, vZ;
    vX(1) = m(1,1); vX(2) = m(2,1); vX(3) = m(3,1);
    vY(1) = m(1,2); vY(2) = m(2,2); vY(3) = m(3,2);
    vZ(1) = m(1,3); vZ(2) = m(2,3); vZ(3) = m(3,3);

    _cp.cp_aRot(1)-=_pInput->GetAxisValue(MOUSE_X_AXIS)*0.5f;
    _cp.cp_aRot(2)-=_pInput->GetAxisValue(MOUSE_Y_AXIS)*0.5f;
    
    if( cam_bMoveForward)      { _cp.cp_vPos -= vZ *cam_fSpeed; };
    if( cam_bMoveBackward)     { _cp.cp_vPos += vZ *cam_fSpeed; };
    if( cam_bMoveLeft)         { _cp.cp_vPos -= vX *cam_fSpeed; };
    if( cam_bMoveRight)        { _cp.cp_vPos += vX *cam_fSpeed; };
    if( cam_bMoveUp)           { _cp.cp_vPos += vY *cam_fSpeed; };
    if( cam_bMoveDown)         { _cp.cp_vPos -= vY *cam_fSpeed; };
    if( cam_bTurnBankingLeft)  { _cp.cp_aRot(3) += 10.0f; };
    if( cam_bTurnBankingRight) { _cp.cp_aRot(3) -= 10.0f; };
    if( cam_bZoomIn)           { _cp.cp_aFOV -= 1.0f; };
    if( cam_bZoomOut)          { _cp.cp_aFOV += 1.0f; };
    if( cam_bZoomDefault)      { _cp.cp_aFOV  = 90.0f; };
    Clamp( _cp.cp_aFOV, 10.0f, 150.0f);

    if( cam_bResetToPlayer) {
      _cp.cp_vPos = pen->GetPlacement().pl_PositionVector;
      _cp.cp_aRot = pen->GetPlacement().pl_OrientationAngle;
    }

    if( cam_bSnapshot) {
      cam_bSnapshot = FALSE;
      WritePos(_cp);
    }

  } else {
    if (!_bInitialized) {
      _bInitialized = TRUE;
      ReadPos(_cp0);
      ReadPos(_cp1);
      SetSpeed(_cp0.cp_fSpeed);
      _fStartTime = _pTimer->CurrentTick();
    }
    TIME tmNow = _pTimer->GetLerpedCurrentTick()-_fStartTime;
    if (tmNow>_cp1.cp_tmTick) {
      _cp0 = _cp1;
      ReadPos(_cp1);
      SetSpeed(_cp0.cp_fSpeed);
    }
    FLOAT fRatio = (tmNow-_cp0.cp_tmTick)/(_cp1.cp_tmTick-_cp0.cp_tmTick);

    _cp.cp_vPos = Lerp(_cp0.cp_vPos, _cp1.cp_vPos, fRatio);
    _cp.cp_aRot = Lerp(_cp0.cp_aRot, _cp1.cp_aRot, fRatio);
    _cp.cp_aFOV = Lerp(_cp0.cp_aFOV, _cp1.cp_aFOV, fRatio);
  }

  CPlacement3D plCamera;
  plCamera.pl_PositionVector = _cp.cp_vPos;
  plCamera.pl_OrientationAngle = _cp.cp_aRot;

  // init projection parameters
  CPerspectiveProjection3D prPerspectiveProjection;
  prPerspectiveProjection.FOVL() = _cp.cp_aFOV;
  prPerspectiveProjection.ScreenBBoxL() = FLOATaabbox2D(
    FLOAT2D(0.0f, 0.0f), FLOAT2D((float)pdp->GetWidth(), (float)pdp->GetHeight())
  );
  prPerspectiveProjection.AspectRatioL() = 1.0f;
  prPerspectiveProjection.FrontClipDistanceL() = 0.3f;

  CAnyProjection3D prProjection;
  prProjection = prPerspectiveProjection;

  // set up viewer position
  prProjection->ViewerPlacementL() = plCamera;
  // render the view
  RenderView(*pen->en_pwoWorld, *(CEntity*)NULL, prProjection, *pdp);
}

BOOL CAM_IsOn(void)
{
  return _bCameraOn;
}


BOOL _bPlayerViewRendered = TRUE;

void GameRedrawView( CDrawPort *pdpDrawPort, ULONG ulFlags)
{
  /*
  // if thumbnail saving has been required
  if( _fnmThumb!="")
  { // reset the need for saving thumbnail
    CTFileName fnm = _fnmThumb;
    _fnmThumb = CTString("");
    // render one game view to a small cloned drawport
    //PIX pixSizeJ = pdpDrawPort->GetHeight();
    PIXaabbox2D boxThumb( PIX2D(0,0), PIX2D(128,128));
    CDrawPort dpThumb( pdpDrawPort, boxThumb);
    _bPlayerViewRendered = FALSE;
    GameRedrawView( &dpThumb, 0);
    if (_bPlayerViewRendered) {
      // grab screenshot
      CImageInfo iiThumb;
      CTextureData tdThumb;
      dpThumb.GrabScreen( iiThumb);
      // try to save thumbnail
      try {
        CTFileStream strmThumb;
        tdThumb.Create_t( &iiThumb, 128, MAX_MEX_LOG2, FALSE);
        strmThumb.Create_t(fnm);
        tdThumb.Write_t( &strmThumb);
        strmThumb.Close();
      } catch( char *strError) {
        // report an error to console, if failed
        CPrintF( "%s\n", strError);
      }
    } else {
      _fnmThumb = fnm;
    }
  }*/

  if( ulFlags) {
    // pretouch memory if required (only if in game mode, not screengrabbing or profiling!)
    SE_PretouchIfNeeded();
  }

  // if game is started and computer isn't on
  //BOOL bClientJoined = FALSE;
  if( runningLevel )
  {
/*
    INDEX ctObservers = Clamp(gam_iObserverConfig, 0, 4);
    INDEX iObserverOffset = ClampDn(gam_iObserverOffset, 0);
    if (gm_CurrentSplitScreenCfg==SSC_OBSERVER) {
      ctObservers = ClampDn(ctObservers, 1);
    }
    if (gm_CurrentSplitScreenCfg!=SSC_OBSERVER) {
      if (!gam_bEnableAdvancedObserving || !GetSP()->sp_bCooperative) {
        ctObservers = 0;
      }
    }
    MakeSplitDrawports(gm_CurrentSplitScreenCfg, ctObservers, pdpDrawPort);

    // get number of local players
    INDEX ctLocals = 0;
    {for (INDEX i=0; i<4; i++) {
      if (gm_lpLocalPlayers[i].lp_pplsPlayerSource!=NULL) {
        ctLocals++;
      }
    }}
*/
    CEntity *apenViewers[7];
    apenViewers[0] = NULL;
    apenViewers[1] = NULL;
    apenViewers[2] = NULL;
    apenViewers[3] = NULL;
    apenViewers[4] = NULL;
    apenViewers[5] = NULL;
    apenViewers[6] = NULL;
    INDEX ctViewers = 0;
/*
    // check if input is enabled
    BOOL bDoPrescan = _pInput->IsInputEnabled() &&
      !_pNetwork->IsPaused() && !_pNetwork->GetLocalPause() &&
      _pShell->GetINDEX("inp_bAllowPrescan");
    // prescan input
    if (bDoPrescan) {
      _pInput->GetInput(TRUE);
    }
    // timer must not occur during prescanning
    { 
#if defined(PLATFORM_UNIX) && !defined(SINGLE_THREADED)
      #warning "This seems to cause Race Condition, so disabled"
#else
      CTSingleLock csTimer(&_pTimer->tm_csHooks, TRUE);
#endif
    // for each local player
    for( INDEX i=0; i<4; i++) {
      // if local player
      CPlayerSource *ppls = gm_lpLocalPlayers[i].lp_pplsPlayerSource;
      if( ppls!=NULL) {
        // get local player entity
        apenViewers[ctViewers++] = _pNetwork->GetLocalPlayerEntity(ppls);
        // precreate action for it for this tick
        if (bDoPrescan) {
          // copy its local controls to current controls
          memcpy(
            ctl_pvPlayerControls,
            gm_lpLocalPlayers[i].lp_ubPlayerControlsState,
            ctl_slPlayerControlsSize);

          // do prescanning
          CPlayerAction paPreAction;
          INDEX iCurrentPlayer = gm_lpLocalPlayers[i].lp_iPlayer;
          CControls &ctrls = gm_actrlControls[ iCurrentPlayer];
          ctrls.CreateAction(gm_apcPlayers[iCurrentPlayer], paPreAction, TRUE);

          // copy the local controls back
          memcpy(
            gm_lpLocalPlayers[i].lp_ubPlayerControlsState,
            ctl_pvPlayerControls,
            ctl_slPlayerControlsSize);
        }
      }
    }}

    // fill in all players that are not local
    INDEX ctNonlocals = 0;
    CEntity *apenNonlocals[16];
    memset(apenNonlocals, 0, sizeof(apenNonlocals));
    {for (INDEX i=0; i<16; i++) {
      CEntity *pen = CEntity::GetPlayerEntity(i);
      if (pen!=NULL && !_pNetwork->IsPlayerLocal(pen)) {
        apenNonlocals[ctNonlocals++] = pen;
      }
    }}

    // if there are any non-local players
    if (ctNonlocals>0) {
      // for each observer
      {for (INDEX i=0; i<ctObservers; i++) {
        // get the given player with given offset that is not local
        INDEX iPlayer = (i+iObserverOffset)%ctNonlocals;
        apenViewers[ctViewers++] = apenNonlocals[iPlayer];
      }}
    }
*/

    apenViewers[ctViewers++] = pen;
    // for each view
    BOOL bHadViewers = FALSE;
    {for (INDEX i=0; i<ctViewers; i++) {
      CDrawPort *pdp = main_dp;
      if (pdp!=NULL && pdp->Lock()) {

        // if there is a viewer
        if (apenViewers[i]!=NULL) {
          bHadViewers = TRUE;
          // if predicted
          if (apenViewers[i]->IsPredicted()) {
            // use predictor instead
            apenViewers[i] = apenViewers[i]->GetPredictor();
          }

          if (!CAM_IsOn()) {
            _bPlayerViewRendered = TRUE;
            // render it
            apenViewers[i]->RenderGameView(pdp, (void*)((size_t)ulFlags));
          } else {
            CAM_Render(apenViewers[i], pdp);
          }
        } else {
          pdp->Fill( C_BLACK|CT_OPAQUE);
        }
        pdp->Unlock();
      }
    }}
    if (!bHadViewers) {
      pdpDrawPort->Lock();
      pdpDrawPort->Fill( C_BLACK|CT_OPAQUE);
      pdpDrawPort->Unlock();
    }
/*
    // create drawport for messages (left on DH)
    CDrawPort dpMsg(pdpDrawPort, TRUE);
    if ((ulFlags&GRV_SHOWEXTRAS) && dpMsg.Lock())
    {
      // print pause indicators
      CTString strIndicator;
      if (_pNetwork->IsDisconnected()) {
        strIndicator.PrintF(TRANSV("Disconnected: %s\nPress F9 to reconnect"), (const char *)_pNetwork->WhyDisconnected());
      } else if (_pNetwork->IsWaitingForPlayers()) {
        strIndicator = TRANS("Waiting for all players to connect");
      } else if (_pNetwork->IsWaitingForServer()) {
        strIndicator = TRANS("Waiting for server to continue");
      } else if (!_pNetwork->IsConnectionStable()) {
        strIndicator = TRANS("Trying to stabilize connection...");
      } else if (_pNetwork->IsGameFinished()) {
        strIndicator = TRANS("Game finished");
      } else if (_pNetwork->IsPaused() || _pNetwork->GetLocalPause()) {
        strIndicator = TRANS("Paused");
      } else if (_tvMenuQuickSave.tv_llValue!=0 && 
        (_pTimer->GetHighPrecisionTimer()-_tvMenuQuickSave).GetSeconds()<3) {
        strIndicator = TRANS("Use F6 for QuickSave during game!");
      } else if (_pNetwork->ga_sesSessionState.ses_strMOTD!="") {
        CTString strMotd = _pNetwork->ga_sesSessionState.ses_strMOTD;
        static CTString strLastMotd = "";
        static CTimerValue tvLastMotd((__int64) 0);
        if (strLastMotd!=strMotd) {
          tvLastMotd = _pTimer->GetHighPrecisionTimer();
          strLastMotd = strMotd;
        }
        if (tvLastMotd.tv_llValue!=((__int64) 0) && (_pTimer->GetHighPrecisionTimer()-tvLastMotd).GetSeconds()<3) {
          strIndicator = strMotd;
        }
      }

      if (strIndicator!="") {
        // setup font
        dpMsg.SetFont( _pfdDisplayFont);
        dpMsg.SetTextAspect( 1.0f);
        dpMsg.PutTextCXY( strIndicator, 
        dpMsg.GetWidth()*0.5f, 
        dpMsg.GetHeight()*0.4f, SE_COL_BLUEGREEN_LT|192);
      }
      // print recording indicator
      if (_pNetwork->IsRecordingDemo()) {
        // setup font
        dpMsg.SetFont( _pfdDisplayFont);
        dpMsg.SetTextScaling( 1.0f);
        dpMsg.SetTextAspect( 1.0f);
        dpMsg.PutText( TRANS("Recording"), 
        dpMsg.GetWidth()*0.1f, 
        dpMsg.GetHeight()*0.1f, C_CYAN|192);
      }

      // print some statistics
      PrintStats( &dpMsg);
  
      // print last few lines from console to top of screen
      if (_pGame->gm_csConsoleState==CS_OFF) ConsolePrintLastLines( &dpMsg);

      // print demo OSD
      if( dem_bOSD) {
        CTString strMessage;
        // print the message
        strMessage.PrintF("%.2fs", _pNetwork->ga_fDemoTimer);
        dpMsg.SetFont( _pfdDisplayFont);
        dpMsg.SetTextAspect( 1.0f);
        dpMsg.PutText( strMessage, 20, 20);
      }
      dpMsg.Unlock();
    }
*//*
    // keep frames' time if required
    if( gm_bProfileDemo)
    {
      CTimerValue tvThisFrame = _pTimer->GetHighPrecisionTimer();
      // if demo has been finished
      if( _pNetwork->IsDemoPlayFinished()) {
        // end profile
        gm_bProfileDemo = FALSE;
        CPrintF( DemoReportAnalyzedProfile());
        CPrintF( "-\n");
      } else {
        // determine frame time delta
        TIME tmDelta = (tvThisFrame - _tvLastFrame).GetSeconds();
        _tvLastFrame =  tvThisFrame;
        _atmFrameTimes.Push() = tmDelta;  // add new frame time stamp
        INDEX *piTriangles = _actTriangles.Push(4); // and polygons count
        piTriangles[0] = _pGfx->gl_ctWorldTriangles;
        piTriangles[1] = _pGfx->gl_ctModelTriangles;
        piTriangles[2] = _pGfx->gl_ctParticleTriangles;
        piTriangles[3] = _pGfx->gl_ctTotalTriangles;
      }
    }
    
    // execute cvar after demoplay
    if( _pNetwork->IsDemoPlayFinished() && dem_strPostExec!="") _pShell->Execute(dem_strPostExec);
    */
  }
  // if no game is active
  else
  {/*
    // clear background
    if( pdpDrawPort->Lock()) {
 	    pdpDrawPort->Fill( SE_COL_BLUE_DARK|CT_OPAQUE);
      pdpDrawPort->FillZBuffer( ZBUF_BACK);
      pdpDrawPort->Unlock();
    }*/
  }
/*
  // check for new chat message
  static INDEX ctChatMessages=0;
  INDEX ctNewChatMessages = _pShell->GetINDEX("net_ctChatMessages");
  if (ctNewChatMessages!=ctChatMessages) {
    ctChatMessages=ctNewChatMessages;
    PlayScriptSound(MAX_SCRIPTSOUNDS-1, CTFILENAME("Sounds\\Menu\\Chat.wav"), 4.0f*gam_fChatSoundVolume, 1.0f, FALSE);
  }
*/
  // update sounds and forbid probing
  _pSound->UpdateSounds();
  _pGfx->gl_bAllowProbing = FALSE;
/*
  if( bSaveScreenShot || dem_iAnimFrame>=0)
  {
    // make the screen shot directory if it doesn't already exist
    bSaveScreenShot = FALSE;
    CTFileName fnmExpanded;
    ExpandFilePath(EFP_WRITE, CTString("ScreenShots"), fnmExpanded);

    // rcg01052002 This is done in Stream.cpp, now. --ryan.
    //_mkdir(fnmExpanded);

    // create a name for screenshot
    CTFileName fnmScreenShot;
    if( dem_iAnimFrame<0) {
      fnmScreenShot = MakeScreenShotName();
    } else {
      // create number for the file
      CTString strNumber;
      strNumber.PrintF("%05d", (INDEX)dem_iAnimFrame);
      fnmScreenShot = CTString("ScreenShots\\Anim_")+strNumber+".tga";
      dem_iAnimFrame+=1;
    }
    // grab screen creating image info
    CImageInfo iiImageInfo;
    if( pdpDrawPort->Lock()) {
      pdpDrawPort->GrabScreen( iiImageInfo, 1);
      pdpDrawPort->Unlock();
    }
    // try to
    try {
      // save screen shot as TGA
      iiImageInfo.SaveTGA_t( fnmScreenShot);
      if( dem_iAnimFrame<0) CPrintF( TRANS("screen shot: %s\n"), (const char *) (CTString&)fnmScreenShot);
    }
    // if failed
    catch (char *strError) {
      // report error
      CPrintF( TRANS("Cannot save screenshot:\n%s\n"), strError);
    }
  }
  */
}


int SubMain(LPSTR lpCmdLine)
{
  if( !Init(lpCmdLine)) return FALSE;

  int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
  manager = new ECSManager();

  CFontData _fdBig;
  CFontData _fdMedium;
  CFontData _fdSmall;
  main_fb_font = new CFontData;
  main_fb_font->Load_t(  CTFILENAME( "Fonts\\Display3-narrow.fnt"));
  _fdMedium.Load_t( CTFILENAME( "Fonts\\Display3-normal.fnt"));
  _fdBig.Load_t(    CTFILENAME( "Fonts\\Display3-caps.fnt"));

  // initialy, application is running and active, console and menu are off
  runningGame = TRUE;
  runningMenu = TRUE;

  position_system = new PositionSystem;
  manager->addSystem((SESystem*)position_system);
  render_system = new RenderSystem;
  manager->addSystem((SESystem*)render_system);
  input_system = new InputSystem;
  manager->addSystem((SESystem*)input_system);

  MenuImage* logosam = new MenuImage();
  logosam->x = 480;
  logosam->y = 10;
  logosam->w = 1024;
  logosam->h = 256;
  logosam->fntex = CTFILENAME("Textures\\Logo\\logo.tex");
  manager->addEntity((SEEntity*)logosam);

  MenuImage* logoct = new MenuImage();
  logoct->x = 16;
  logoct->y = 864;
  logoct->w = 200;
  logoct->h = 200;
  logoct->fntex = CTFILENAME("Textures\\Logo\\LogoCT.tex");
  manager->addEntity((SEEntity*)logoct);

  MenuImage* logogg = new MenuImage(); 
  logogg->x = 1704;
  logogg->y = 864;
  logogg->w = 200;
  logogg->h = 200;
  logogg->fntex = CTFILENAME("Textures\\Logo\\GodGamesLogo.tex");
  manager->addEntity((SEEntity*)logogg);

  MenuButton* menu_button_sp = new MenuButton;
  menu_button_sp->y = 300;
  menu_button_sp->w = 300;
  menu_button_sp->h = 50;
  menu_button_sp->align = 0;
  menu_button_sp->align_x = 0;
  menu_button_sp->fontdata = _fdBig;
  menu_button_sp->fontsize = 2;
  menu_button_sp->textmode = 0;
  menu_button_sp->str = TRANS("SINGLE PLAYER");
  menu_button_sp->function = startGame;
  menu_button_sp->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_sp->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_sp);

  MenuButton* menu_button_net = new MenuButton;
  menu_button_net->y = 375;
  menu_button_net->w = 300;
  menu_button_net->h = 50;
  menu_button_net->align = 0;
  menu_button_net->align_x = 0;
  menu_button_net->fontdata = _fdBig;
  menu_button_net->fontsize = 2;
  menu_button_net->textmode = 0;
  menu_button_net->str = TRANS("NETWORK");
  menu_button_net->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_net->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_net);

  MenuButton* menu_button_split = new MenuButton;
  menu_button_split->y = 450;
  menu_button_split->w = 300;
  menu_button_split->h = 50;
  menu_button_split->align = 0;
  menu_button_split->align_x = 0;
  menu_button_split->fontdata = _fdBig;
  menu_button_split->fontsize = 2;
  menu_button_split->textmode = 0;
  menu_button_split->str = TRANS("SPLIT SCREEN");
  menu_button_split->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_split->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_split);

  MenuButton* menu_button_demo = new MenuButton;
  menu_button_demo->y = 525;
  menu_button_demo->w = 300;
  menu_button_demo->h = 50;
  menu_button_demo->align = 0;
  menu_button_demo->align_x = 0;
  menu_button_demo->fontdata = _fdBig;
  menu_button_demo->fontsize = 2;
  menu_button_demo->textmode = 0;
  menu_button_demo->str = TRANS("DEMO");
  menu_button_demo->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_demo->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_demo);

  MenuButton* menu_button_mod = new MenuButton;
  menu_button_mod->y = 600;
  menu_button_mod->w = 300;
  menu_button_mod->h = 50;
  menu_button_mod->align = 0;
  menu_button_mod->align_x = 0;
  menu_button_mod->fontdata = _fdBig;
  menu_button_mod->fontsize = 2;
  menu_button_mod->textmode = 0;
  menu_button_mod->str = TRANS("MODS");
  menu_button_mod->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_mod->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_mod);

  MenuButton* menu_button_hs = new MenuButton;
  menu_button_hs->y = 675;
  menu_button_hs->w = 300;
  menu_button_hs->h = 50;
  menu_button_hs->align = 0;
  menu_button_hs->align_x = 0;
  menu_button_hs->fontdata = _fdBig;
  menu_button_hs->fontsize = 2;
  menu_button_hs->textmode = 0;
  menu_button_hs->str = TRANS("HIGH SCORES");
  menu_button_hs->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_hs->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_hs);

  MenuButton* menu_button_opt = new MenuButton;
  menu_button_opt->y = 750;
  menu_button_opt->w = 300;
  menu_button_opt->h = 50;
  menu_button_opt->align = 0;
  menu_button_opt->align_x = 0;
  menu_button_opt->fontdata = _fdBig;
  menu_button_opt->fontsize = 2;
  menu_button_opt->textmode = 0;
  menu_button_opt->str = TRANS("OPTIONS");
  menu_button_opt->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_opt->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_opt);

  MenuButton* menu_button_quit = new MenuButton;
  menu_button_quit->y = 825;
  menu_button_quit->w = 300;
  menu_button_quit->h = 50;
  menu_button_quit->align = 0;
  menu_button_quit->align_x = 0;
  menu_button_quit->fontdata = _fdBig;
  menu_button_quit->fontsize = 2;
  menu_button_quit->textmode = 0;
  menu_button_quit->str = TRANS("QUIT");
  menu_button_quit->function = quitgame;
  menu_button_quit->color = SE_COL_ORANGE_LIGHT|255;
  menu_button_quit->color2 = SE_COL_ORANGE_DARK|255;
  manager->addEntity((SEEntity*)menu_button_quit);

  int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

  FOREACHINDYNAMICCONTAINER(*manager->systems,SESystem,system) {
    system->init();
  }
  int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
  fprintf(stderr,"Loading time t1: %ld\n", t1-t0);
  fprintf(stderr,"Loading time t2: %ld\n", t2-t1);
  int64_t tloop1;
  int64_t tloop2;
  int64_t ticks = 0;
  while(runningGame) // start of game loop
  {
      tloop1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
      if( !pMainWin->isIconic() ) {
          update();
          if(main_dp->Lock()) {
              // clear z-buffer
              main_dp->FillZBuffer(ZBUF_BACK);
              main_dp->Fill(SE_COL_ORANGE_NEUTRAL|0xff);
              GameRedrawView(main_dp,1);
              FOREACHINDYNAMICCONTAINER(*manager->systems,SESystem,system) {
                  system->update();
              }
              main_dp->Unlock();
              main_vp->SwapBuffers();
          }
      }
      tloop2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
      fprintf(stderr,"Execute time tick(%ld): %ld\n", ticks++, tloop2-tloop1);
  }// end of game loop
  return TRUE;
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