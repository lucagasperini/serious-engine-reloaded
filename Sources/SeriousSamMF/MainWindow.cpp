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
    drawPort = NULL;
    viewPort = NULL;
}

SEMainWindow::~SEMainWindow()
{

}

// close the main application window
void SEMainWindow::destroy()
{
    if( viewPort != NULL) {
        _pGfx->DestroyWindowCanvas(viewPort);
        viewPort = NULL;
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
    // try to set new display mode
    _pGfx->SetDisplayMode( api, adapter, w, h, depth );

    pWindow = SEInterfaceSDL::createWindow(title, x, y, w, h, api, mode, resizable, status);

    if( pWindow == NULL) FatalError(TRANS("Cannot open main window!"));
    
    SE_UpdateWindowHandle(pWindow);

    // create canvas
    ASSERT( viewPort == NULL);
    _pGfx->CreateWindowCanvas( pWindow, &viewPort, &drawPort);

    // erase context of both buffers (for the sake of wide-screen)
    if( drawPort!=NULL && drawPort->Lock()) {
      drawPort->Fill(C_BLACK|CT_OPAQUE);
      drawPort->Unlock();
      viewPort->SwapBuffers();
      drawPort->Lock();
      drawPort->Fill(C_BLACK|CT_OPAQUE);
      drawPort->Unlock();
      viewPort->SwapBuffers();
    }
/*
    // lets try some wide screen screaming :)
    const PIX pixYBegAdj = pdp->GetHeight() * 21/24;
    const PIX pixYEndAdj = pdp->GetHeight() * 3/24;
    const PIX pixXEnd    = pdp->GetWidth();
    pdpWideScreen = new CDrawPort( pdp, PIXaabbox2D( PIX2D(0,pixYBegAdj), PIX2D(pixXEnd, pixYEndAdj)));
    pdpWideScreen->dp_fWideAdjustment = 9.0f / 12.0f;
    if( sam_bWideScreen) pdp = pdpWideScreen;
*/
    // initial screen fill and swap, just to get context running
    if( drawPort!=NULL && drawPort->Lock()) {
      drawPort->Fill(SE_COL_ORANGE_NEUTRAL|255);
      drawPort->Unlock();
      viewPort->SwapBuffers();
    }
    return TRUE;
}

BOOL SEMainWindow::isIconic()
{
    return IsIconic(pWindow);
}