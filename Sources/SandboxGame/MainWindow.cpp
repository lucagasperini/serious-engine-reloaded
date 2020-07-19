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

#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Engine.h>

#include "Define.h"
#include "MainWindow.h"

extern CDrawPort* g_drawport;
extern CViewPort* g_viewport;

SEMainWindow::SEMainWindow()
{
    /* Set default values */
    x = SDL_WINDOWPOS_CENTERED;
    y = SDL_WINDOWPOS_CENTERED;
    api = GfxAPIType::GAT_OGL;
    flags = SE_MAINWINDOW_FLAGS_NULL;
}

SEMainWindow::~SEMainWindow()
{
}

// close the main application window
void SEMainWindow::destroy()
{
    if (g_viewport != NULL) {
        _pGfx->DestroyWindowCanvas(g_viewport);
        g_viewport = NULL;
    }
    // if window exists
    if (pWindow != NULL) {
        // destroy it
        SDL_DestroyWindow((SDL_Window*)pWindow);
        pWindow = NULL;
    }
}

BOOL SEMainWindow::create()
{
    if (pWindow != NULL)
        destroy();

    // try to set new display mode
    _pGfx->SetDisplayMode(api, adapter, w, h, depth);

    ULONG tmp_flags = flags;

    if (api == GAT_OGL && !(tmp_flags & SDL_WINDOW_OPENGL))
        tmp_flags = tmp_flags | SDL_WINDOW_OPENGL;

    pWindow = SDL_CreateWindow(title, x, y, w, h, tmp_flags);

    if (pWindow == NULL)
        FatalError(TRANS("Cannot open main window!"));

    SE_UpdateWindowHandle(pWindow);

#ifndef DEBUG
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif

    _pGfx->CreateWindowCanvas(pWindow, &g_viewport, &g_drawport);

    // initial screen fill and swap, just to get context running
    if (g_drawport != NULL && g_drawport->Lock()) {
        g_drawport->Fill(SE_COL_ORANGE_NEUTRAL | 255);
        g_drawport->Unlock();
        g_viewport->SwapBuffers();
    }

    return TRUE;
}

BOOL SEMainWindow::isIconic()
{
    return IsIconic(pWindow);
}