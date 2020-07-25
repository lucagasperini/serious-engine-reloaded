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

#include <thread>

// FIXME: Cant compile without this global variable
HWND _hwndMain = NULL;

static char* argv0 = NULL;

extern void load_all_game_system();
extern void load_all_game_entity();

CTFileName g_world_file = CTFILENAME("Levels\\TestGame.wld");
CWorld* g_world_data = NULL;
CTString g_gamename = "serioussammf";

CTFileName g_logfile = CTFILENAME("fast.log");
CTFileStream g_logstream;

COLOR g_fb_color = C_BLACK | 0xff;

BOOL g_dbg_draw_border = FALSE;
BOOL g_dbg_draw_id = FALSE;
BOOL g_dbg_draw_position = FALSE;
BOOL g_dbg_draw_fps = FALSE;
BOOL g_dbg_draw_cursor = FALSE;

UINT g_resolution_width = 1280;
UINT g_resolution_height = 720;

UINT g_virtual_resolution_width = 1920;
UINT g_virtual_resolution_height = 1080;

BOOL g_game_started = FALSE;
BOOL g_window_started = FALSE;

int submain(char* _cmdline)
{
    SESplashScreen splashscreen;
    splashscreen.setBitmap("Splash.bmp");
    splashscreen.show();

    // initialize engine
    SE_InitEngine(argv0, g_gamename);

    SE_LoadDefaultFonts();

    int64_t t0 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    // Add space for 1 MB + 256 Event pointers (32/64 bit per pointer)
    SER::Manager::init(1048576, 256);

    load_all_game_system();
    load_all_game_entity();

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    ULONG number_thread = 1;
    SER::Manager::setThreadNumber(number_thread);
    SER::Manager::splitThreadMemory();

    g_window_started = TRUE;

    g_world_data = new CWorld;
    g_world_data->Load_t(g_world_file);

    int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    printf("Loading time t1: %ld\n", t1 - t0);
    printf("Loading time t2: %ld\n", t2 - t1);

    splashscreen.hide();

    while (g_window_started) {

        // start of game loop
        g_game_started = TRUE;
        SER::Manager::run();

        SER::Manager::quit();
    }
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