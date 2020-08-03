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

#include "MainWindow.h"
#include "ECSGame/Component.h"
#include <ECS/Manager.h>
#include <SDL.h>

using namespace SER;

MainWindow::MainWindow()
{
    window = NULL;
    api = GfxAPIType::GAT_OGL;
    flags = 0;
    adapter = 0;
    depth = DisplayDepth::DD_32BIT;
    dp = NULL;
    vp = NULL;
}

MainWindow::~MainWindow()
{
    destroy();
    window = NULL;
    dp = NULL;
    vp = NULL;
}

void MainWindow::init()
{
    if (window != NULL) {
        destroy();
    }

    SER_GET_SETTING_ARG(resolution, UINT, SC_RESOLUTION);
    SER_GET_SETTING_ARG(is_fullscreen, BOOL, SC_FULLSCREEN);

    // try to set new display mode
    _pGfx->SetDisplayMode(api, adapter,
        resolution[0], resolution[1], depth);

    ULONG tmp_flags = flags;

    if (api == GAT_OGL && !(tmp_flags & SDL_WINDOW_OPENGL))
        tmp_flags = tmp_flags | SDL_WINDOW_OPENGL;

    if (*is_fullscreen)
        tmp_flags = tmp_flags | SDL_WINDOW_FULLSCREEN;
    else
        tmp_flags = tmp_flags & ~SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        resolution[0], resolution[1], tmp_flags);

    if (window == NULL)
        FatalError(TRANS("Cannot open main window!"));

    SE_UpdateWindowHandle(window);

    _pGfx->CreateWindowCanvas(window, &vp, &dp);

    SER_GET_SETTING_ARG(debug_grabmouse, BOOL, SC_DEBUG_GRABMOUSE);
    if (*debug_grabmouse)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    else
        SDL_SetRelativeMouseMode(SDL_FALSE);

    // initial screen fill and swap, just to get context running
    if (dp != NULL && dp->Lock()) {
        dp->Fill(C_BLACK | 255);
        dp->Unlock();
        vp->SwapBuffers();
    }
}

void MainWindow::destroy()
{
    if (vp != NULL) {
        _pGfx->DestroyWindowCanvas(vp);
        vp = NULL;
    }
    // if window exists
    if (window != NULL) {
        // destroy it
        SDL_DestroyWindow((SDL_Window*)window);
        window = NULL;
    }
}