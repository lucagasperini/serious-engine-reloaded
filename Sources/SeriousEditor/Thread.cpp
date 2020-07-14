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

#include "Thread.h"

#include <SDL.h>

#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Engine.h>

CTString main_gamename = "serioussammf";
CTString cmdline;

CDrawPort* g_drawport = NULL;
CViewPort* g_viewport = NULL;

INDEX main_win_mode = 0;
GfxAPIType main_win_api = GfxAPIType::GAT_OGL; // 0==OpenGL
INDEX main_win_width = 1280; // current size of the window
INDEX main_win_height = 720; // current size of the window
DisplayDepth main_win_depth = DisplayDepth::DD_32BIT; // 0==default, 1==16bit, 2==32bit
INDEX main_win_adapter = 0;

BOOL main_game_started = FALSE;

static SDL_Window* sdl_window;
static SDL_Renderer* sdl_renderer;
static SDL_Surface* sdl_surface;
static void* window_id;

extern char* argv0;

SEThread::SEThread(QWidget* _win)
{
    CTSTREAM_BEGIN
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
            FatalError("SDL_Init(VIDEO|AUDIO) failed. Reason: [%s].", SDL_GetError());
        }
        //atexit(quit);
        SDL_Init(SDL_INIT_JOYSTICK); // don't care if this fails.

        // initialize engine
        SE_InitEngine(argv0, main_gamename);

        SE_LoadDefaultFonts();

        // try to set new display mode
        _pGfx->SetDisplayMode(main_win_api, main_win_adapter, main_win_width, main_win_height, main_win_depth);

        sdl_window = SDL_CreateWindowFrom((void*)_win->winId());
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);

        if (sdl_window == NULL)
            FatalError(TRANS("Cannot open main window!"));

        SE_UpdateWindowHandle(sdl_window);

        _pGfx->CreateWindowCanvas(sdl_window, &g_viewport, &g_drawport);

        main_game_started = TRUE;
    }
    CTSTREAM_END;
}

void SEThread::run()
{
    CTSTREAM_BEGIN
    {
        while (main_game_started) {
            // initial screen fill and swap, just to get context running
            if (g_drawport != NULL && g_drawport->Lock()) {
                g_drawport->Fill(0xee9c0000 | 255);
                g_drawport->Unlock();
                g_viewport->SwapBuffers();
            }
        }
    }
    CTSTREAM_END;
}
