#include <SDL.h>

#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Engine.h>

#include "Colors.h"
#include "MainWindow.h"

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
    if (vp != NULL) {
        _pGfx->DestroyWindowCanvas(vp);
        vp = NULL;
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

    _pGfx->CreateWindowCanvas(pWindow, &vp, &dp);

    // initial screen fill and swap, just to get context running
    if (dp != NULL && dp->Lock()) {
        dp->Fill(SE_COL_ORANGE_NEUTRAL | 255);
        dp->Unlock();
        vp->SwapBuffers();
    }

    return TRUE;
}

BOOL SEMainWindow::isIconic()
{
    return IsIconic(pWindow);
}