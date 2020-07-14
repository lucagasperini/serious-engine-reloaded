// Copyright (C) 2020 Luca Gasperini <luca.gasperini@xsoftware.it>
//
// This file is part of Serious Engine Reloaded.
//
// Serious Engine Reloaded is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Serious Engine Reloaded is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Serious Engine Reloaded.  If not, see <http://www.gnu.org/licenses/>.

#include <SDL.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Base/Stream.h>
#include <Engine/Engine.h>
#include <Engine/Sound/SoundLibrary.h>

#include "Colors.h"
#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "ECS/Manager.h"
#include "MainWindow.h"
#include "SplashScreen.h"

#define SSMF_WINDOW_RECOVERY_W 640
#define SSMF_WINDOW_RECOVERY_H 480

static char* argv0 = NULL;

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */
ENGINE_API extern INDEX snd_iFormat;

FLOAT3D world_start_position = FLOAT3D(9, 6, 32);
ANGLE3D world_start_rotation = ANGLE3D(0, 20, 0);
CTFileName world_file = CTFILENAME("Levels\\TestGame.wld");
CWorld* world_data = NULL;
CTString main_gamename = "serioussammf";

SESplashScreen* scr_splashscreen = NULL;
SEMainWindow* main_win = NULL;
CDrawPort* main_dp = NULL;
CViewPort* main_vp = NULL;
ECSManager* manager = NULL;

CFontData* main_font_small;
CFontData* main_font_medium;
CFontData* main_font_big;

COLOR fallback_color = C_BLACK | 0xff;

BOOL dbg_draw_border = FALSE;
BOOL dbg_draw_id = FALSE;
BOOL dbg_draw_position = FALSE;
BOOL dbg_draw_fps = FALSE;
ULONG dbg_count_fps = 0;

UINT game_vresolution_width = 1920;
UINT game_vresolution_height = 1080;
// display mode settings
ULONG main_win_flags = SE_MAINWINDOW_FLAGS_NULL;
GfxAPIType main_win_api = GfxAPIType::GAT_OGL; // 0==OpenGL
ULONG main_win_width = 1280; // current size of the window
ULONG main_win_height = 720; // current size of the window
DisplayDepth main_win_depth = DisplayDepth::DD_32BIT;
INDEX main_win_adapter = 0;

// list of possible display modes for recovery
const INDEX fallback_win_modes[][3] = {
    // color, API, adapter
    { DD_DEFAULT, GAT_OGL, 0 },
    { DD_16BIT, GAT_OGL, 0 },
    { DD_16BIT, GAT_OGL, 1 }, // 3dfx Voodoo2
};
const INDEX fallback_win_modes_counter = ARRAYCOUNT(fallback_win_modes);

BOOL main_game_started = FALSE;

void quitgame()
{
    main_game_started = FALSE;
}

void resolution_fullscreen()
{
    ULONG current_flags = main_win->getFlags();
    if (current_flags & SDL_WINDOW_FULLSCREEN)
        main_win->setFlags(current_flags ^ SDL_WINDOW_FULLSCREEN);
    else
        main_win->setFlags(current_flags | SDL_WINDOW_FULLSCREEN);

    main_dp->Unlock();
    main_win->create();
    main_dp = main_win->getDrawPort();
    main_vp = main_win->getViewPort();
    main_dp->Lock();
}

void resolution_change(UINT w, UINT h)
{
    if (main_win->getW() != w || main_win->getH() != h) {
        game_vresolution_width = main_win->getW();
        game_vresolution_height = main_win->getH();
        main_dp->Unlock();
        main_win->setW(w);
        main_win->setH(h);
        main_win->create();
        main_dp = main_win->getDrawPort();
        main_vp = main_win->getViewPort();
        main_dp->Lock();
    }
}

BOOL init(CTString strCmdLine)
{
    scr_splashscreen = new SESplashScreen();
    main_win = new SEMainWindow();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
        FatalError("SDL_Init(VIDEO|AUDIO) failed. Reason: [%s].", SDL_GetError());
        return FALSE;
    }

    SDL_Init(SDL_INIT_JOYSTICK); // don't care if this fails.

    scr_splashscreen->show();

    // parse command line before initializing engine
    // FIXME: Maybe add this in future
    // ParseCommandLine(strCmdLine);

    // initialize engine
    SE_InitEngine(argv0, main_gamename);

    SE_LoadDefaultFonts();

    // initialize sound library
    snd_iFormat = Clamp(snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
    _pSound->SetFormat((enum CSoundLibrary::SoundFormat)snd_iFormat);

    // apply application mode
    main_win->setTitle(main_gamename);
    main_win->setW(main_win_width);
    main_win->setH(main_win_height);
    main_win->setDepth(DisplayDepth::DD_DEFAULT);
    main_win->setAdapter(main_win_adapter);
    BOOL result = main_win->create();
    main_dp = main_win->getDrawPort();
    main_vp = main_win->getViewPort();

    if (!result) {
        main_win->setW(SSMF_WINDOW_RECOVERY_W);
        main_win->setH(SSMF_WINDOW_RECOVERY_H);
        for (int i = 0; i < fallback_win_modes_counter; i++) {
            main_win->setDepth((DisplayDepth)fallback_win_modes[i][0]);
            main_win->setAPI((GfxAPIType)fallback_win_modes[i][1]);
            main_win->setAdapter(fallback_win_modes[i][2]);
            CPrintF(TRANSV("\nTrying recovery mode %d...\n"), i);
            result = main_win->create();
            if (result)
                break;
        }
    }
    // if all failed
    if (!result) {
        FatalError(TRANS(
            "Cannot set display mode!\n"
            "Serious Sam was unable to find display mode with hardware acceleration.\n"
            "Make sure you install proper drivers for your video card as recommended\n"
            "in documentation and set your desktop to 16 bit (65536 colors).\n"
            "Please see ReadMe file for troubleshooting information.\n"));
    }

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

int submain(char* cmdline)
{
    if (!init(cmdline))
        return FALSE;

    int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    manager = new ECSManager();

    main_font_small = new CFontData;
    main_font_medium = new CFontData;
    main_font_big = new CFontData;

    main_font_small->Load_t(CTFILENAME("Fonts\\Display3-narrow.fnt"));
    main_font_medium->Load_t(CTFILENAME("Fonts\\Display3-normal.fnt"));
    main_font_big->Load_t(CTFILENAME("Fonts\\Display3-caps.fnt"));

    GameControl* game_control = new GameControl;
    memset(game_control->kb_keybind, 0, sizeof(ULONG) * SE_ECS_KEYBIND_MAX);
    game_control->kb_keybind[SE_KEYBIND_FULLSCREEN] = SDLK_F1;
    game_control->kb_keybind[SE_KEYBIND_RESOLUTION_VGA] = SDLK_F2;
    game_control->kb_keybind[SE_KEYBIND_RESOLUTION_SVGA] = SDLK_F3;
    game_control->kb_keybind[SE_KEYBIND_RESOLUTION_WXGA] = SDLK_F4;
    game_control->kb_keybind[SE_KEYBIND_RESOLUTION_HD] = SDLK_F5;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_BORDER] = SDLK_F6;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_ENTITYID] = SDLK_F7;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_POSITION] = SDLK_F8;
    game_control->kb_keybind[SE_KEYBIND_DEBUG_FPS] = SDLK_F9;
    manager->addEntity((SEEntity*)game_control);

    Camera* camera = new Camera();
    camera->cam_fov = 90.0f;
    camera->cam_pos = world_start_position;
    camera->cam_rot = world_start_rotation;
    camera->cam_speed = 1.0f;
    memset(camera->kb_keybind, 0, sizeof(ULONG) * SE_ECS_KEYBIND_MAX);
    camera->kb_keybind[SE_KEYBIND_CAMERA_RESET] = SDLK_F10;
    camera->kb_keybind[SE_KEYBIND_CAMERA_RIGHT] = SDLK_RIGHT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_LEFT] = SDLK_LEFT;
    camera->kb_keybind[SE_KEYBIND_CAMERA_FORWARD] = SDLK_UP;
    camera->kb_keybind[SE_KEYBIND_CAMERA_BACK] = SDLK_DOWN;
    camera->kb_keybind[SE_KEYBIND_CAMERA_UP] = SDLK_SPACE;
    camera->kb_keybind[SE_KEYBIND_CAMERA_DOWN] = SDLK_c;
    manager->addEntity(camera);

    MenuImage* logosam = new MenuImage();
    logosam->pos_x = 480;
    logosam->pos_y = 10;
    logosam->pos_w = 1024;
    logosam->pos_h = 256;
    logosam->tex_file = CTFILENAME("Textures\\Logo\\logo.tex");
    manager->addEntity((SEEntity*)logosam);

    MenuImage* logoct = new MenuImage();
    logoct->pos_x = 16;
    logoct->pos_y = 864;
    logoct->pos_w = 200;
    logoct->pos_h = 200;
    logoct->tex_file = CTFILENAME("Textures\\Logo\\LogoCT.tex");
    manager->addEntity((SEEntity*)logoct);

    MenuImage* logose = new MenuImage();
    logose->pos_x = 1704;
    logose->pos_y = 864;
    logose->pos_w = 200;
    logose->pos_h = 200;
    logose->tex_file = CTFILENAME("Textures\\Logo\\GodGamesLogo.tex");
    manager->addEntity((SEEntity*)logose);

    MenuButton* menu_button_sp = new MenuButton;
    menu_button_sp->pos_y = 300;
    menu_button_sp->pos_w = 300;
    menu_button_sp->pos_h = 50;
    menu_button_sp->txt_align = 0;
    menu_button_sp->align_x = 0;
    menu_button_sp->txt_fontdata = *main_font_big;
    menu_button_sp->txt_fontsize = 2;
    menu_button_sp->txt_mode = 0;
    menu_button_sp->txt_str = TRANS("SINGLE PLAYER");
    menu_button_sp->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_sp->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_sp);

    MenuButton* menu_button_net = new MenuButton;
    menu_button_net->pos_y = 375;
    menu_button_net->pos_w = 300;
    menu_button_net->pos_h = 50;
    menu_button_net->txt_align = 0;
    menu_button_net->align_x = 0;
    menu_button_net->txt_fontdata = *main_font_big;
    menu_button_net->txt_fontsize = 2;
    menu_button_net->txt_mode = 0;
    menu_button_net->txt_str = TRANS("NETWORK");
    menu_button_net->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_net->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_net);

    MenuButton* menu_button_split = new MenuButton;
    menu_button_split->pos_y = 450;
    menu_button_split->pos_w = 300;
    menu_button_split->pos_h = 50;
    menu_button_split->txt_align = 0;
    menu_button_split->align_x = 0;
    menu_button_split->txt_fontdata = *main_font_big;
    menu_button_split->txt_fontsize = 2;
    menu_button_split->txt_mode = 0;
    menu_button_split->txt_str = TRANS("SPLIT SCREEN");
    menu_button_split->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_split->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_split);

    MenuButton* menu_button_demo = new MenuButton;
    menu_button_demo->pos_y = 525;
    menu_button_demo->pos_w = 300;
    menu_button_demo->pos_h = 50;
    menu_button_demo->txt_align = 0;
    menu_button_demo->align_x = 0;
    menu_button_demo->txt_fontdata = *main_font_big;
    menu_button_demo->txt_fontsize = 2;
    menu_button_demo->txt_mode = 0;
    menu_button_demo->txt_str = TRANS("DEMO");
    menu_button_demo->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_demo->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_demo);

    MenuButton* menu_button_mod = new MenuButton;
    menu_button_mod->pos_y = 600;
    menu_button_mod->pos_w = 300;
    menu_button_mod->pos_h = 50;
    menu_button_mod->txt_align = 0;
    menu_button_mod->align_x = 0;
    menu_button_mod->txt_fontdata = *main_font_big;
    menu_button_mod->txt_fontsize = 2;
    menu_button_mod->txt_mode = 0;
    menu_button_mod->txt_str = TRANS("MODS");
    menu_button_mod->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_mod->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_mod);

    MenuButton* menu_button_hs = new MenuButton;
    menu_button_hs->pos_y = 675;
    menu_button_hs->pos_w = 300;
    menu_button_hs->pos_h = 50;
    menu_button_hs->txt_align = 0;
    menu_button_hs->align_x = 0;
    menu_button_hs->txt_fontdata = *main_font_big;
    menu_button_hs->txt_fontsize = 2;
    menu_button_hs->txt_mode = 0;
    menu_button_hs->txt_str = TRANS("HIGH SCORES");
    menu_button_hs->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_hs->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_hs);

    MenuButton* menu_button_opt = new MenuButton;
    menu_button_opt->pos_y = 750;
    menu_button_opt->pos_w = 300;
    menu_button_opt->pos_h = 50;
    menu_button_opt->txt_align = 0;
    menu_button_opt->align_x = 0;
    menu_button_opt->txt_fontdata = *main_font_big;
    menu_button_opt->txt_fontsize = 2;
    menu_button_opt->txt_mode = 0;
    menu_button_opt->txt_str = TRANS("OPTIONS");
    menu_button_opt->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_opt->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_opt);

    MenuButton* menu_button_quit = new MenuButton;
    menu_button_quit->pos_y = 825;
    menu_button_quit->pos_w = 300;
    menu_button_quit->pos_h = 50;
    menu_button_quit->txt_align = 0;
    menu_button_quit->align_x = 0;
    menu_button_quit->txt_fontdata = *main_font_big;
    menu_button_quit->txt_fontsize = 2;
    menu_button_quit->txt_mode = 0;
    menu_button_quit->txt_str = TRANS("QUIT");
    menu_button_quit->sea_action = quitgame;
    menu_button_quit->txt_color = SE_COL_ORANGE_LIGHT | 255;
    menu_button_quit->btn_color2 = SE_COL_ORANGE_DARK | 255;
    manager->addEntity((SEEntity*)menu_button_quit);

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    manager->init();

    world_data = new CWorld;
    world_data->Load_t(world_file);

    int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    fprintf(stderr, "Loading time t1: %ld\n", t1 - t0);
    fprintf(stderr, "Loading time t2: %ld\n", t2 - t1);
    int64_t tloop1;
    int64_t tloop2;
    int64_t ticks = 0;

    main_game_started = TRUE;

    while (main_game_started) // start of game loop
    {
        tloop1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
        if (!main_win->isIconic()) {
            if (main_dp->Lock()) {
                // clear z-buffer
                main_dp->FillZBuffer(ZBUF_BACK);
                main_dp->Fill(SE_COL_ORANGE_NEUTRAL | 0xff);

                manager->update();

                main_dp->Unlock();
                main_vp->SwapBuffers();
            }
        }
        tloop2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
        fprintf(stderr, "Execute time tick(%ld): %ld\n", ticks++, tloop2 - tloop1);
        dbg_count_fps = 1000 / (tloop2 - tloop1);
    } // end of game loop
    return TRUE;
}

int main(int _argc, char** _argv)
{
    argv0 = _argv[0];

    CTString cmdline;
    for (int i = 1; i < _argc; i++) {
        cmdline += " \"";
        cmdline += _argv[i];
        cmdline += "\"";
    }

    int result;
    CTSTREAM_BEGIN
    {
        result = submain((char*)((const char*)cmdline));
    }
    CTSTREAM_END;

    return result;
}