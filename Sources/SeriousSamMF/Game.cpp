/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// Game.cpp : Defines the initialization routines for the DLL.
//

#include <sys/timeb.h>
#include <time.h>
#include <locale.h>

#ifdef PLATFORM_WIN32
#include <direct.h> // for _mkdir()
#include <io.h>
#endif

#include <Engine/Engine.h>
#include <Engine/Base/Statistics.h>
#include <Engine/CurrentVersion.h>

#include "Game.h"

#define SE_COL_ORANGE_LIGHT     0xffd70000
#define SE_COL_ORANGE_NEUTRAL   0xee9c0000
#define SE_COL_ORANGE_DARK      0x9b4b0000
#define SE_COL_ORANGE_DARK_LT   0xbc6a0000
#define SE_COL_BLUE_DARK_HV     0x151c2300
#define SE_COL_BLUE_DARK        0x2a384600
#define SE_COL_BLUE_DARK_LT     0x43596f00
#define SE_COL_BLUE_NEUTRAL     0x5c7a9900
#define SE_COL_BLUE_NEUTRAL_LT  0x6097cc00
#define SE_COL_BLUE_LIGHT       0x64b4ff00
#define SE_COL_BLUEGREEN_LT     0x6cff6c00

FLOAT con_fHeightFactor = 0.5f;
FLOAT con_tmLastLines   = 5.0f;
INDEX con_bTalk = 0;
CTimerValue _tvMenuQuickSave((__int64) 0);

// used filenames
CTFileName fnmPersistentSymbols = CTString("Scripts\\PersistentSymbols.ini");
CTFileName fnmStartupScript     = CTString("Scripts\\Game_startup.ini");
CTFileName fnmConsoleHistory    = CTString("Temp\\ConsoleHistory.txt");
CTFileName fnmCommonControls    = CTString("Controls\\System\\Common.ctl");

// force dependency for player class
DECLARE_CTFILENAME( fnmPlayerClass, "Classes\\Player.ecl");

#define MAX_HIGHSCORENAME 16
#define MAX_HIGHSCORETABLESIZE ((MAX_HIGHSCORENAME+1+sizeof(INDEX)*4)*HIGHSCORE_COUNT)*2
//UBYTE _aubHighScoreBuffer[MAX_HIGHSCORETABLESIZE];
//UBYTE _aubHighScorePacked[MAX_HIGHSCORETABLESIZE];

// controls used for all commands not belonging to any particular player
//static CControls _ctrlCommonControls;

// array for keeping all frames' times
static CStaticStackArray<TIME>  _atmFrameTimes;
static CStaticStackArray<INDEX> _actTriangles;  // world, model, particle, total


SEGame::SEGame()
{

}

SEGame::~SEGame()
{

}


// recorded profiling stats
static CTimerValue _tvDemoStarted;
static CTimerValue _tvLastFrame;
static CTString _strProfile;
static BOOL  _bDumpNextTime = FALSE;
static BOOL  _bStartProfilingNextTime = FALSE;
static BOOL  _bProfiling = FALSE;
static INDEX _ctProfileRecording = 0;
static FLOAT gam_iRecordHighScore = -1.0f;

FLOAT gam_afAmmoQuantity[5]        = {2.0f,  2.0f,  1.0f, 1.0f , 2.0f };
FLOAT gam_afDamageStrength[5]      = {0.25f, 0.5f,  1.0f, 1.5f , 2.0f };
FLOAT gam_afEnemyAttackSpeed[5]    = {0.75f, 0.75f, 1.0f, 2.0f , 2.0f };
FLOAT gam_afEnemyMovementSpeed[5]  = {1.0f , 1.0f , 1.0f, 1.25f, 1.25f};
FLOAT gam_fManaTransferFactor = 0.5f;
FLOAT gam_fExtraEnemyStrength          = 0;
FLOAT gam_fExtraEnemyStrengthPerPlayer = 0;
INDEX gam_iCredits = -1;   // number of credits for respawning
FLOAT gam_tmSpawnInvulnerability = 3;
INDEX gam_iScoreLimit = 100000;
INDEX gam_iFragLimit = 20;
INDEX gam_iTimeLimit = 0;
INDEX gam_bWeaponsStay = TRUE;
INDEX gam_bAmmoStays = TRUE;
INDEX gam_bHealthArmorStays = TRUE;
INDEX gam_bAllowHealth = TRUE;
INDEX gam_bAllowArmor = TRUE;
INDEX gam_bInfiniteAmmo = FALSE;
INDEX gam_bRespawnInPlace = TRUE;
INDEX gam_bPlayEntireGame = TRUE;
INDEX gam_bFriendlyFire = FALSE;
INDEX gam_ctMaxPlayers = 8;
INDEX gam_bWaitAllPlayers = FALSE;
INDEX gam_iInitialMana = 100;
INDEX gam_bQuickLoad = FALSE;
INDEX gam_bQuickSave = FALSE;
INDEX gam_iQuickSaveSlots = 8;

INDEX gam_iQuickStartDifficulty = 1;
INDEX gam_iQuickStartMode = 0;
INDEX gam_bQuickStartMP = 0;

INDEX gam_bEnableAdvancedObserving = 0;
INDEX gam_iObserverConfig = 0;
INDEX gam_iObserverOffset = 0;

INDEX gam_iStartDifficulty = 1;
INDEX gam_iStartMode = 0;
CTString gam_strGameAgentExtras = "";

INDEX gam_iBlood = 2;     // 0=none, 1=green, 2=red, 3=hippie
INDEX gam_bGibs  = TRUE;   

INDEX gam_bUseExtraEnemies = TRUE;

static INDEX hud_iEnableStats = 1;
static FLOAT hud_fEnableFPS   = 1;
static INDEX hud_iStats    = 0;
static INDEX hud_bShowTime = FALSE;
static INDEX hud_bShowClock = FALSE;
static INDEX hud_bShowNetGraph = FALSE;
static INDEX hud_bShowResolution = FALSE;
static INDEX dem_bOSD         = FALSE;
static INDEX dem_bPlay        = FALSE;
static INDEX dem_bPlayByName  = FALSE;
static INDEX dem_bProfile     = FALSE;
static INDEX dem_iProfileRate = 5;
static CTString dem_strPostExec = "";

static INDEX ctl_iCurrentPlayerLocal = -1;
static INDEX ctl_iCurrentPlayer = -1;
static FLOAT gam_fChatSoundVolume = 0.25f;

BOOL map_bIsFirstEncounter = FALSE;
BOOL _bUserBreakEnabled = FALSE;

COLOR SEGame::LCDGetColor(COLOR colDefault, const char *strName)
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

/*
 * Global game object (in our case Flesh) initialization function
 */
int SEGame::init(const CTFileName &fnGameSettings)
{
  /*
  gam_strCustomLevel = ""; // filename of custom level chosen
  gam_strSessionName = TRANS("Unnamed session"); // name of multiplayer network session
  gam_strJoinAddress = TRANS("serveraddress");   // join address

  gm_MenuSplitScreenCfg    = SSC_PLAY1;
  gm_StartSplitScreenCfg   = SSC_PLAY1;
  gm_CurrentSplitScreenCfg = SSC_PLAY1;
  gm_iLastSetHighScore = 0;
  gm_iSinglePlayer = 0;
  gm_iWEDSinglePlayer = 0;
  gm_bGameOn = FALSE;
  gm_bMenuOn = FALSE;
  gm_bFirstLoading = FALSE;
  gm_bProfileDemo = FALSE;
  gm_slPlayerControlsSize = 0;
  gm_pvGlobalPlayerControls = NULL;
  memset(gm_aiMenuLocalPlayers, 0, sizeof(gm_aiMenuLocalPlayers));
  memset(gm_aiStartLocalPlayers, 0, sizeof(gm_aiStartLocalPlayers));

  // first assign translated to make dependcy catcher extract the translations
  gm_astrAxisNames[AXIS_MOVE_UD] = TRANS("move u/d");      
  gm_astrAxisNames[AXIS_MOVE_LR] = TRANS("move l/r");      
  gm_astrAxisNames[AXIS_MOVE_FB] = TRANS("move f/b");      
  gm_astrAxisNames[AXIS_TURN_UD] = TRANS("look u/d");      
  gm_astrAxisNames[AXIS_TURN_LR] = TRANS("turn l/r");      
  gm_astrAxisNames[AXIS_TURN_BK] = TRANS("banking");       
  gm_astrAxisNames[AXIS_LOOK_UD] = TRANS("view u/d");      
  gm_astrAxisNames[AXIS_LOOK_LR] = TRANS("view l/r");      
  gm_astrAxisNames[AXIS_LOOK_BK] = TRANS("view banking");  
  // but we must not really use the translation for loading
  gm_astrAxisNames[AXIS_MOVE_UD] = "move u/d";     // 
  gm_astrAxisNames[AXIS_MOVE_LR] = "move l/r";     // 
  gm_astrAxisNames[AXIS_MOVE_FB] = "move f/b";     // 
  gm_astrAxisNames[AXIS_TURN_UD] = "look u/d";     // 
  gm_astrAxisNames[AXIS_TURN_LR] = "turn l/r";     // 
  gm_astrAxisNames[AXIS_TURN_BK] = "banking";      // 
  gm_astrAxisNames[AXIS_LOOK_UD] = "view u/d";     // 
  gm_astrAxisNames[AXIS_LOOK_LR] = "view l/r";     // 
  gm_astrAxisNames[AXIS_LOOK_BK] = "view banking"; // 

  gm_csConsoleState = CS_OFF;
  gm_csComputerState = CS_OFF;

  gm_bGameOn = FALSE;
  gm_bMenuOn = FALSE;
  gm_bFirstLoading = FALSE;
  gm_aiMenuLocalPlayers[0] =  0;
  gm_aiMenuLocalPlayers[1] = -1;
  gm_aiMenuLocalPlayers[2] = -1;
  gm_aiMenuLocalPlayers[3] = -1;

  gm_MenuSplitScreenCfg = SSC_PLAY1;

  LoadPlayersAndControls();

  gm_iWEDSinglePlayer = 0;
  gm_iSinglePlayer = 0;

  // add game timer handler
  _pTimer->AddHandler(&m_gthGameTimerHandler);
  // add shell variables
  _pShell->DeclareSymbol("user void RecordProfile(void);",        (void *)&RecordProfile);
  _pShell->DeclareSymbol("user void SaveScreenShot(void);",       (void *)&SaveScreenShot);
  _pShell->DeclareSymbol("user void DumpProfileToConsole(void);", (void *)&DumpProfileToConsole);
  _pShell->DeclareSymbol("user void DumpProfileToFile(void);",    (void *)&DumpProfileToFile);
  _pShell->DeclareSymbol("user INDEX hud_iStats;", (void *)&hud_iStats);
  _pShell->DeclareSymbol("user INDEX hud_bShowResolution;", (void *)&hud_bShowResolution);
  _pShell->DeclareSymbol("persistent user INDEX hud_bShowTime;",  (void *)&hud_bShowTime);
  _pShell->DeclareSymbol("persistent user INDEX hud_bShowClock;", (void *)&hud_bShowClock);
  _pShell->DeclareSymbol("user INDEX dem_bOnScreenDisplay;", (void *)&dem_bOSD);
  _pShell->DeclareSymbol("user INDEX dem_bPlay;",            (void *)&dem_bPlay);
  _pShell->DeclareSymbol("user INDEX dem_bPlayByName;",      (void *)&dem_bPlayByName);
  _pShell->DeclareSymbol("user INDEX dem_bProfile;",         (void *)&dem_bProfile);
  _pShell->DeclareSymbol("user INDEX dem_iAnimFrame;",       (void *)&dem_iAnimFrame);
  _pShell->DeclareSymbol("user CTString dem_strPostExec;",   (void *)&dem_strPostExec);
  _pShell->DeclareSymbol("persistent user INDEX dem_iProfileRate;",  (void *)&dem_iProfileRate);
  _pShell->DeclareSymbol("persistent user INDEX hud_bShowNetGraph;", (void *)&hud_bShowNetGraph);
  _pShell->DeclareSymbol("FLOAT gam_afEnemyMovementSpeed[5];", (void *)&gam_afEnemyMovementSpeed);
  _pShell->DeclareSymbol("FLOAT gam_afEnemyAttackSpeed[5];",   (void *)&gam_afEnemyAttackSpeed);
  _pShell->DeclareSymbol("FLOAT gam_afDamageStrength[5];",     (void *)&gam_afDamageStrength);
  _pShell->DeclareSymbol("FLOAT gam_afAmmoQuantity[5];",       (void *)&gam_afAmmoQuantity);
  _pShell->DeclareSymbol("persistent user FLOAT gam_fManaTransferFactor;", (void *)&gam_fManaTransferFactor);
  _pShell->DeclareSymbol("persistent user FLOAT gam_fExtraEnemyStrength         ;", (void *)&gam_fExtraEnemyStrength          );
  _pShell->DeclareSymbol("persistent user FLOAT gam_fExtraEnemyStrengthPerPlayer;", (void *)&gam_fExtraEnemyStrengthPerPlayer );
  _pShell->DeclareSymbol("persistent user INDEX gam_iInitialMana;",        (void *)&gam_iInitialMana);
  _pShell->DeclareSymbol("persistent user INDEX gam_iScoreLimit;",  (void *)&gam_iScoreLimit);
  _pShell->DeclareSymbol("persistent user INDEX gam_iFragLimit;",   (void *)&gam_iFragLimit);
  _pShell->DeclareSymbol("persistent user INDEX gam_iTimeLimit;",   (void *)&gam_iTimeLimit);
  _pShell->DeclareSymbol("persistent user INDEX gam_ctMaxPlayers;", (void *)&gam_ctMaxPlayers);
  _pShell->DeclareSymbol("persistent user INDEX gam_bWaitAllPlayers;", (void *)&gam_bWaitAllPlayers);
  _pShell->DeclareSymbol("persistent user INDEX gam_bFriendlyFire;",   (void *)&gam_bFriendlyFire);
  _pShell->DeclareSymbol("persistent user INDEX gam_bPlayEntireGame;", (void *)&gam_bPlayEntireGame);
  _pShell->DeclareSymbol("persistent user INDEX gam_bWeaponsStay;",    (void *)&gam_bWeaponsStay);

  _pShell->DeclareSymbol("persistent user INDEX gam_bAmmoStays       ;", (void *)&gam_bAmmoStays       );
  _pShell->DeclareSymbol("persistent user INDEX gam_bHealthArmorStays;", (void *)&gam_bHealthArmorStays);
  _pShell->DeclareSymbol("persistent user INDEX gam_bAllowHealth     ;", (void *)&gam_bAllowHealth     );
  _pShell->DeclareSymbol("persistent user INDEX gam_bAllowArmor      ;", (void *)&gam_bAllowArmor      );
  _pShell->DeclareSymbol("persistent user INDEX gam_bInfiniteAmmo    ;", (void *)&gam_bInfiniteAmmo    );
  _pShell->DeclareSymbol("persistent user INDEX gam_bRespawnInPlace  ;", (void *)&gam_bRespawnInPlace  );

  _pShell->DeclareSymbol("persistent user INDEX gam_iCredits;", (void *)&gam_iCredits);
  _pShell->DeclareSymbol("persistent user FLOAT gam_tmSpawnInvulnerability;", (void *)&gam_tmSpawnInvulnerability);

  _pShell->DeclareSymbol("persistent user INDEX gam_iBlood;", (void *)&gam_iBlood);
  _pShell->DeclareSymbol("persistent user INDEX gam_bGibs;",  (void *)&gam_bGibs);

  _pShell->DeclareSymbol("persistent user INDEX gam_bUseExtraEnemies;",  (void *)&gam_bUseExtraEnemies);

  _pShell->DeclareSymbol("user INDEX gam_bQuickLoad;", (void *)&gam_bQuickLoad);
  _pShell->DeclareSymbol("user INDEX gam_bQuickSave;", (void *)&gam_bQuickSave);
  _pShell->DeclareSymbol("user INDEX gam_iQuickSaveSlots;", (void *)&gam_iQuickSaveSlots);
  _pShell->DeclareSymbol("user INDEX gam_iQuickStartDifficulty;", (void *)&gam_iQuickStartDifficulty);
  _pShell->DeclareSymbol("user INDEX gam_iQuickStartMode;",       (void *)&gam_iQuickStartMode);
  _pShell->DeclareSymbol("user INDEX gam_bQuickStartMP;",       (void *)&gam_bQuickStartMP);
  _pShell->DeclareSymbol("persistent user INDEX gam_iStartDifficulty;", (void *)&gam_iStartDifficulty);
  _pShell->DeclareSymbol("persistent user INDEX gam_iStartMode;",       (void *)&gam_iStartMode);
  _pShell->DeclareSymbol("persistent user CTString gam_strGameAgentExtras;", (void *)&gam_strGameAgentExtras);
  _pShell->DeclareSymbol("persistent user CTString gam_strCustomLevel;", (void *)&gam_strCustomLevel);
  _pShell->DeclareSymbol("persistent user CTString gam_strSessionName;", (void *)&gam_strSessionName);
  _pShell->DeclareSymbol("persistent user CTString gam_strJoinAddress;", (void *)&gam_strJoinAddress);
  _pShell->DeclareSymbol("persistent user INDEX gam_bEnableAdvancedObserving;", (void *)&gam_bEnableAdvancedObserving);
  _pShell->DeclareSymbol("user INDEX gam_iObserverConfig;", (void *)&gam_iObserverConfig);
  _pShell->DeclareSymbol("user INDEX gam_iObserverOffset;", (void *)&gam_iObserverOffset);

  _pShell->DeclareSymbol("INDEX gam_iRecordHighScore;", (void *)&gam_iRecordHighScore);

  _pShell->DeclareSymbol("persistent user FLOAT con_fHeightFactor;", (void *)&con_fHeightFactor);
  _pShell->DeclareSymbol("persistent user FLOAT con_tmLastLines;",   (void *)&con_tmLastLines);
  _pShell->DeclareSymbol("user INDEX con_bTalk;", (void *)&con_bTalk);
  _pShell->DeclareSymbol("user void ReportDemoProfile(void);", (void *)&ReportDemoProfile);
  _pShell->DeclareSymbol("user void DumpDemoProfile(void);",   (void *)&DumpDemoProfile);
  extern CTString GetGameAgentRulesInfo(void);
  extern CTString GetGameTypeName(INDEX);
  extern CTString GetGameTypeNameCfunc(void* pArgs);
  extern CTString GetCurrentGameTypeName(void);
  extern ULONG GetSpawnFlagsForGameType(INDEX);
  extern ULONG GetSpawnFlagsForGameTypeCfunc(void* pArgs);
  extern BOOL IsMenuEnabled_(const CTString &);
  extern BOOL IsMenuEnabledCfunc(void* pArgs);
  _pShell->DeclareSymbol("user CTString GetGameAgentRulesInfo(void);",   (void *)&GetGameAgentRulesInfo);
  _pShell->DeclareSymbol("user CTString GetGameTypeName(INDEX);",        (void *)&GetGameTypeNameCfunc);
  _pShell->DeclareSymbol("user CTString GetCurrentGameTypeName(void);",  (void *)&GetCurrentGameTypeName);
  _pShell->DeclareSymbol("user INDEX GetSpawnFlagsForGameType(INDEX);",  (void *)&GetSpawnFlagsForGameTypeCfunc);
  _pShell->DeclareSymbol("user INDEX IsMenuEnabled(CTString);",          (void *)&IsMenuEnabledCfunc);
  _pShell->DeclareSymbol("user void Say(CTString);",                     (void *)&Say);
  _pShell->DeclareSymbol("user void SayFromTo(INDEX, INDEX, CTString);", (void *)&SayFromTo);

  _pShell->DeclareSymbol("CTString GetGameTypeNameSS(INDEX);",           (void *)&GetGameTypeName);
  _pShell->DeclareSymbol("INDEX GetSpawnFlagsForGameTypeSS(INDEX);",     (void *)&GetSpawnFlagsForGameType);
  _pShell->DeclareSymbol("INDEX IsMenuEnabledSS(CTString);",             (void *)&IsMenuEnabled_);

  _pShell->DeclareSymbol("user const INDEX ctl_iCurrentPlayerLocal;", (void *)&ctl_iCurrentPlayerLocal);
  _pShell->DeclareSymbol("user const INDEX ctl_iCurrentPlayer;",      (void *)&ctl_iCurrentPlayer);

  _pShell->DeclareSymbol("user FLOAT gam_fChatSoundVolume;",      (void *)&gam_fChatSoundVolume);

  _pShell->DeclareSymbol("user void PlaySound(INDEX, CTString, FLOAT, FLOAT, INDEX);", (void *)&PlayScriptSound);
  _pShell->DeclareSymbol("user void StopSound(INDEX);", (void *)&StopScriptSound);
  _pShell->DeclareSymbol("user INDEX IsSoundPlaying(INDEX);", (void *)&IsScriptSoundPlaying);

  CAM_Init();

  // load persistent symbols
  if (!_bDedicatedServer) {
    _pShell->Execute(CTString("include \"")+fnmPersistentSymbols+"\";");
  }
  // execute the startup script
  _pShell->Execute(CTString("include \"")+fnmStartupScript+"\";");

  // check the size and pointer of player control variables that are local to each player
  if (ctl_slPlayerControlsSize<=0
    ||ctl_slPlayerControlsSize>sizeof(((CLocalPlayer*)NULL)->lp_ubPlayerControlsState)
    ||ctl_pvPlayerControls==NULL) {
    FatalError(TRANS("Current player controls are invalid."));
  }

  // load common controls
  try {
    _ctrlCommonControls.Load_t(fnmCommonControls);
  } catch (char * strError) {
    //FatalError(TRANS("Cannot load common controls: %s\n"), strError);
  }

  // init LCD textures/fonts
  LCDInit();

  // load console history
  CTString strConsole;
  try {
    strConsole.LoadKeepCRLF_t(fnmConsoleHistory);
    gam_strConsoleInputBuffer = strConsole;
  } catch (char *strError) {
    (void)strError; // must ignore if there is no history file
  }

  // load game shell settings
  try {
    Load_t();
  } catch (char *strError) {
    CPrintF(TRANSV("Cannot load game settings:\n%s\n  Using defaults\n"), strError);
  }

  CON_DiscardLastLineTimes();

  // provide URL to the engine
  _strModURL = "http://www.croteam.com/mods/TheSecondEncounter";
  */
}
