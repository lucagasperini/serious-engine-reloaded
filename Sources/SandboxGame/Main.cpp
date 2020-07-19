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

#include "SplashScreen.h"
#include <ECS/Manager.h>

#include "ECSGame/Entity.h"
#include "ECSGame/RenderSystem.h"

// FIXME: Cant compile without this global variable
HWND _hwndMain = NULL;

static char* argv0 = NULL;

extern void load_all_game_system();
extern void load_all_game_entity();

CTFileName g_world_file = CTFILENAME("Levels\\TestGame.wld");
CWorld* g_world_data = NULL;
CTString g_gamename = "serioussammf";

CDrawPort* g_drawport = NULL;
CViewPort* g_viewport = NULL;

CTFileName g_logfile = CTFILENAME("fast.log");
CTFileStream g_logstream;

COLOR g_fb_color = C_BLACK | 0xff;

BOOL g_dbg_draw_border = FALSE;
BOOL g_dbg_draw_id = FALSE;
BOOL g_dbg_draw_position = FALSE;
BOOL g_dbg_draw_fps = FALSE;
BOOL g_dbg_draw_cursor = FALSE;

UINT g_vresolution_width = 1920;
UINT g_vresolution_height = 1080;

BOOL g_game_started = FALSE;

//TODO: This two function are called on Control System, but main_win is not a global variable.
void g_resolution_fullscreen()
{
    /*
    ULONG current_flags = main_win->getFlags();
    if (current_flags & SDL_WINDOW_FULLSCREEN)
        main_win->setFlags(current_flags ^ SDL_WINDOW_FULLSCREEN);
    else
        main_win->setFlags(current_flags | SDL_WINDOW_FULLSCREEN);

    //g_drawport->Unlock();
    main_win->create();
    //g_drawport->Lock();
    */
}

void g_resolution_change(UINT _w, UINT _h)
{
    /*
    if (main_win->getW() != _w || main_win->getH() != _h) {
        g_vresolution_width = main_win->getW();
        g_vresolution_height = main_win->getH();
        //g_drawport->Unlock();
        main_win->setW(_w);
        main_win->setH(_h);
        main_win->create();
        //g_drawport->Lock();
    }
    */
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

int submain(char* _cmdline)
{
    SESplashScreen splashscreen;
    splashscreen.setBitmap("Splash.bmp");
    splashscreen.show();

    // initialize engine
    SE_InitEngine(argv0, g_gamename);

    SE_LoadDefaultFonts();

    int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    load_all_game_system();
    RenderSystem* render_system = new RenderSystem;
    load_all_game_entity();

    ULONG number_thread = 4;
    ECSManager::setThreadNumber(number_thread);
    ECSManager::splitThreadMemory();

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    while (SEEntity* entity = ECSManager::getEntity()) {
        render_system->init(entity);
    }

    g_world_data = new CWorld;
    g_world_data->Load_t(g_world_file);

    int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    printf("Loading time t1: %ld\n", t1 - t0);
    printf("Loading time t2: %ld\n", t2 - t1);
    int64_t tloop1;
    int64_t tloop2;
    int64_t ticks = 0;

    splashscreen.hide();
    g_game_started = TRUE;

    // start of game loop
    ECSManager::run();
    while (g_game_started) {

        render_system->preupdate();

        while (SEEntity* entity = ECSManager::getEntity()) {
            render_system->update(entity);
        }

        render_system->postupdate();
    }
    ECSManager::quit();
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