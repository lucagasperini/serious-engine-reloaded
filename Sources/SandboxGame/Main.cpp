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

struct keybind {
    ULONG key;
    SEEvent event;
};

keybind a_keybind[SER_KEYBIND_MAX];

void fetch_input()
{
    int x = 0;
    int y = 0;
    int delta_x = 0;
    int delta_y = 0;
    int old_x = 0;
    int old_y = 0;
    int* event_parameter_mouse = new int[4];
    int* event_parameter_mouse_click = new int[3];
    SDL_Event event;
    while (g_game_started) {
        old_x = x;
        old_y = y;
        // get real cursor position
        SDL_GetMouseState(&x, &y);

        if (x != old_x || y != old_y) {
            delta_x = old_x - x;
            delta_y = old_y - y;
            event_parameter_mouse[0] = x;
            event_parameter_mouse[1] = y;
            event_parameter_mouse[2] = delta_x;
            event_parameter_mouse[3] = delta_y;
            ECSManager::addEvent(SER_EVENT_MOUSE_MOVE, event_parameter_mouse);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                g_game_started = FALSE;
                g_window_started = FALSE;
            }
            if (event.type == SDL_KEYDOWN) {
                for (UINT i = 0; i < SER_KEYBIND_MAX; i++) {
                    if (a_keybind[i].key == event.key.keysym.sym && event.key.keysym.sym != 0) {
                        ECSManager::addEvent(a_keybind[i].event);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                event_parameter_mouse_click[0] = x;
                event_parameter_mouse_click[1] = y;
                event_parameter_mouse_click[2] = event.button.button;
                ECSManager::addEvent(SER_EVENT_MOUSE_BUTTON, event_parameter_mouse_click);
            }
        }
    }
}

void load_keybind()
{
    memset(a_keybind, 0, sizeof(keybind) * SER_KEYBIND_MAX);
    a_keybind[0].event.code = SER_EVENT_FULLSCREEN_CHANGE;
    a_keybind[0].key = SDLK_F1;
    UINT* p_res_vga = new UINT[2] { 640, 480 };
    a_keybind[1].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[1].event.parameter = p_res_vga;
    a_keybind[1].key = SDLK_F2;
    UINT* p_res_svga = new UINT[2] { 800, 600 };
    a_keybind[2].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[2].event.parameter = p_res_svga;
    a_keybind[2].key = SDLK_F3;
    UINT* p_res_wxga = new UINT[2] { 1280, 720 };
    a_keybind[3].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[3].event.parameter = p_res_wxga;
    a_keybind[3].key = SDLK_F4;
    UINT* p_res_hd = new UINT[2] { 1920, 1080 };
    a_keybind[4].event.code = SER_EVENT_RESOLUTION_CHANGE;
    a_keybind[4].event.parameter = p_res_hd;
    a_keybind[4].key = SDLK_F5;
}

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
    load_all_game_entity();

    load_keybind();

    ULONG number_thread = 1;
    ECSManager::setThreadNumber(number_thread);
    ECSManager::splitThreadMemory();

    int64_t t1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

    BYTE* tmp_ptr = ECSManager::getFirst();

    g_window_started = TRUE;
    splashscreen.hide();
    SEEvent* event = 0;

    while (g_window_started) {

        g_world_data = new CWorld;
        g_world_data->Load_t(g_world_file);

        int64_t t2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
        printf("Loading time t1: %ld\n", t1 - t0);
        printf("Loading time t2: %ld\n", t2 - t1);
        int64_t tloop1;
        int64_t tloop2;
        int64_t ticks = 0;

        // start of game loop
        g_game_started = TRUE;
        std::thread input_thread = std::thread(fetch_input);
        ECSManager::run();

        ECSManager::quit();
        input_thread.join();
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