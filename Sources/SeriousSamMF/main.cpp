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
#include "Controls.h"
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"
#include "ECS/Manager.h"
#include "ECS/RenderSystem.h"
#include "ECS/PositionSystem.h"
#include "ECS/InputSystem.h"
#include "ECS/ControlSystem.h"

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
SEMainWindow* main_win = NULL;
CDrawPort* main_dp = NULL;
CViewPort* main_vp = NULL;
ECSManager* manager = NULL;

PositionSystem* position_system = NULL;
RenderSystem* render_system = NULL;
InputSystem* input_system = NULL;
ControlSystem* control_system = NULL;

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

BOOL main_menu_started;
BOOL main_game_started;
BOOL runningLevel;






BOOL Init(CTString strCmdLine)
{
  scr_splashscreen = new SESplashScreen();
  main_win = new SEMainWindow();
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

  // initialize sound library
  snd_iFormat = Clamp( snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
  _pSound->SetFormat( (enum CSoundLibrary::SoundFormat)snd_iFormat);

  // apply application mode
  main_win->setTitle(sam_strGameName);
  main_win->setW(iWindowW);
  main_win->setH(iWindowH);
  main_win->setDepth(DisplayDepth::DD_DEFAULT);
  main_win->setAdapter(iWindowAdapter);
  BOOL winResult = main_win->create();
  main_dp = main_win->getDrawPort();
  main_vp = main_win->getViewPort();

  if(! winResult) {
    main_win->setW(SSMF_WINDOW_RECOVERY_W);
    main_win->setH(SSMF_WINDOW_RECOVERY_H);
    for(int i = 0; i < ctDefaultModes; i++) {
      main_win->setDepth((DisplayDepth) aDefaultModes[i][0]);
      main_win->setAPI( (GfxAPIType)  aDefaultModes[i][1]);
      main_win->setAdapter(aDefaultModes[i][2]);
      CPrintF(TRANSV("\nTrying recovery mode %d...\n"), i);
      winResult = main_win->create();
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

  scr_splashscreen->hide();

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
void startGame()
{
    if(!runningLevel) {
        CTString strSessionName = "sessione";
        CTFileName fnmWorld = CTFILENAME("Levels\\TestGame.wld");

        UBYTE session_properties[NET_MAXSESSIONPROPERTIES];
        _pNetwork->StartPeerToPeer_t( strSessionName, fnmWorld, 
              0, 1, FALSE, &session_properties);

        pen = new CPlayerEntity();
        pen->en_pwoWorld = &_pNetwork->ga_World;
        pen->en_plPlacement = CPlacement3D(FLOAT3D(370,40,0),ANGLE3D(-80,20,0));

        runningLevel = TRUE;
    }
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


  if( ulFlags) {
    // pretouch memory if required (only if in game mode, not screengrabbing or profiling!)
    SE_PretouchIfNeeded();
  }

  if (runningLevel)
  {

    CDrawPort *pdp = main_dp;
    if (pdp != NULL && pdp->Lock())
    {

      // if there is a viewer
      if (pen != NULL)
      {
        if (!CAM_IsOn())
        {
          _bPlayerViewRendered = TRUE;
          // render it
          pen->RenderGameView(pdp, (void *)((size_t)ulFlags));
        }
        else
        {
          CAM_Render(pen, pdp);
        }
      }
      else
      {
        pdp->Fill(C_BLACK | CT_OPAQUE);
      }
      pdp->Unlock();
    }
  }
  // update sounds and forbid probing
  _pSound->UpdateSounds();
  _pGfx->gl_bAllowProbing = FALSE;
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
  main_game_started = TRUE;
  main_menu_started = TRUE;

  position_system = new PositionSystem;
  manager->addSystem((SESystem*)position_system);
  render_system = new RenderSystem;
  manager->addSystem((SESystem*)render_system);
  input_system = new InputSystem;
  manager->addSystem((SESystem*)input_system);
  control_system = new ControlSystem;
  manager->addSystem((SESystem*)control_system);

  load_game_controls();

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
  menu_button_sp->sea_action = startGame;
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
  menu_button_quit->sea_action = quitgame;
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
  while(main_game_started) // start of game loop
  {
      tloop1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
      if( !main_win->isIconic() ) {
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

  return iResult;
}