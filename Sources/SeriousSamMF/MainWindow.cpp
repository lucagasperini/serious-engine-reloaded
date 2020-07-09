#include <SDL.h>

#include <Engine/Engine.h>
#include <Engine/Base/SDL/SDLEvents.h>

#include "MainWindow.h"
#include "InterfaceSDL.h"
#include "Colors.h"

SEMainWindow::SEMainWindow()
{
    /* Set default values */
    x = SEInterfaceSDL::posWinCentered();
    y = SEInterfaceSDL::posWinCentered();
    api = GfxAPIType::GAT_OGL;
    mode = SE_WINDOW_MODE_WINDOWED;
    resizable = FALSE;
    status = SE_WINDOW_STATUS_NORMAL;
    adapter = 0;
}

SEMainWindow::~SEMainWindow()
{

}

// close the main application window
void SEMainWindow::destroy()
{
    if( vp != NULL) {
        _pGfx->DestroyWindowCanvas(vp);
        vp = NULL;
    }
    // if window exists
    if( pWindow!=NULL) {
        // destroy it
        SDL_DestroyWindow((SDL_Window *) pWindow);
        pWindow = NULL;
    }
}

BOOL SEMainWindow::create()
{
    if(pWindow != NULL)
        destroy();
        
    // try to set new display mode
    _pGfx->SetDisplayMode( api, adapter, w, h, depth );

    pWindow = SEInterfaceSDL::createWindow(title, x, y, w, h, api, mode, resizable, status);

    if( pWindow == NULL) FatalError(TRANS("Cannot open main window!"));
    
    SE_UpdateWindowHandle(pWindow);

    _pGfx->CreateWindowCanvas( pWindow, &vp, &dp);
    
    // initial screen fill and swap, just to get context running
    if( dp!=NULL && dp->Lock()) {
      dp->Fill(SE_COL_ORANGE_NEUTRAL|255);
      dp->Unlock();
      vp->SwapBuffers();
    }

    return TRUE;
}

BOOL SEMainWindow::isIconic()
{
    return IsIconic(pWindow);
}