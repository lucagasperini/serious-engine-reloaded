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

#include "MainWindow.h"
#include "SplashScreen.h"
#include <ECS/Component.h>
#include <ECS/Entity.h>
#include <ECS/Manager.h>

#include "Entity.h"
#include "Global.h"

// FIXME: Cant compile without this global variable
HWND _hwndMain = NULL;
extern INDEX snd_iFormat;

extern BOOL g_game_started;
extern CDrawPort* g_drawport;
extern CViewPort* g_viewport;
extern UINT g_vresolution_width;
extern UINT g_vresolution_height;

extern const INDEX gm_win_fb_mode[][SER_WINDOW_FALLBACK_COUNT];

extern CTString g_gamename;

extern CTFileName g_world_file;
extern CWorld* g_world_data;

extern ECSManager* g_manager;

SEMainWindow* main_win = NULL;
static char* argv0 = NULL;

//TODO: This two function are called on Control System, but main_win is not a global variable.
void g_resolution_fullscreen()
{
    ULONG current_flags = main_win->getFlags();
    if (current_flags & SDL_WINDOW_FULLSCREEN)
        main_win->setFlags(current_flags ^ SDL_WINDOW_FULLSCREEN);
    else
        main_win->setFlags(current_flags | SDL_WINDOW_FULLSCREEN);

    //g_drawport->Unlock();
    main_win->create();
    //g_drawport->Lock();
}

void g_resolution_change(UINT _w, UINT _h)
{
    if (main_win->getW() != _w || main_win->getH() != _h) {
        g_vresolution_width = main_win->getW();
        g_vresolution_height = main_win->getH();
        //g_drawport->Unlock();
        main_win->setW(_w);
        main_win->setH(_h);
        main_win->create();
        //g_drawport->Lock();
    }
}
/*
BOOL init(CTString _cmdline)
{
    // display mode settings
    ULONG win_flags = SE_MAINWINDOW_FLAGS_NULL;
    GfxAPIType win_api = GfxAPIType::GAT_OGL;
    UINT win_width = 1280;
    UINT win_height = 720;
    DisplayDepth win_depth = DisplayDepth::DD_32BIT;
    INDEX win_adapter = 0;

    // list of possible display modes for recovery
    const INDEX m_win_fb_mode[][SER_WINDOW_FALLBACK_COUNT] = {
        // color, API, adapter
        { DD_DEFAULT, GAT_OGL, 0 },
        { DD_16BIT, GAT_OGL, 0 },
        { DD_16BIT, GAT_OGL, 1 }, // 3dfx Voodoo2
    };

    

    main_win = new SEMainWindow();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
        FatalError("SDL_Init(VIDEO|AUDIO) failed. Reason: [%s].", SDL_GetError());
        return FALSE;
    }

    SDL_Init(SDL_INIT_JOYSTICK); // don't care if this fails.

    splashscreen.show();

    // parse command line before initializing engine
    // TODO: Maybe add this in future
    // ParseCommandLine(_cmdline);

    // initialize engine
    SE_InitEngine(argv0, g_gamename);

    SE_LoadDefaultFonts();

    // initialize sound library
    snd_iFormat = Clamp(snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
    _pSound->SetFormat((enum CSoundLibrary::SoundFormat)snd_iFormat);

    // apply application mode
    main_win->setTitle(g_gamename);
    main_win->setW(win_width);
    main_win->setH(win_height);
    main_win->setDepth(DisplayDepth::DD_DEFAULT);
    main_win->setAdapter(win_adapter);
    BOOL result = main_win->create();

    if (!result) {
        main_win->setW(SE_WINDOW_RECOVERY_W);
        main_win->setH(SE_WINDOW_RECOVERY_H);
        for (int i = 0; i < SER_WINDOW_FALLBACK_COUNT; i++) {
            main_win->setDepth((DisplayDepth)m_win_fb_mode[i][0]);
            main_win->setAPI((GfxAPIType)m_win_fb_mode[i][1]);
            main_win->setAdapter(m_win_fb_mode[i][2]);
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

    splashscreen.hide();

    return TRUE;
}
*/
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

int submain(char* _cmdline)
{
    //if (!init(_cmdline))
    //    return FALSE;
    SESplashScreen splashscreen;
    splashscreen.setBitmap("Splash.bmp");
    splashscreen.show();

    // initialize engine
    SE_InitEngine(argv0, g_gamename);

    SE_LoadDefaultFonts();

    int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    // initialize sound library
    snd_iFormat = Clamp(snd_iFormat, (INDEX)CSoundLibrary::SF_NONE, (INDEX)CSoundLibrary::SF_44100_16);
    _pSound->SetFormat((enum CSoundLibrary::SoundFormat)snd_iFormat);

    g_manager = new ECSManager();
    load_all_game_system();
    load_all_game_entity();

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    //g_manager->init();

    g_world_data = new CWorld;
    g_world_data->Load_t(g_world_file);

    int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    fprintf(stderr, "Loading time t1: %ld\n", t1 - t0);
    fprintf(stderr, "Loading time t2: %ld\n", t2 - t1);
    int64_t tloop1;
    int64_t tloop2;
    int64_t ticks = 0;

    g_game_started = TRUE;
    splashscreen.hide();

    //while (g_game_started) // start of game loop
    //{
    //    if (!main_win->isIconic()) {
    //        g_manager->update();
    //    }
    //
    //} // end of game loop
    g_manager->update();
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