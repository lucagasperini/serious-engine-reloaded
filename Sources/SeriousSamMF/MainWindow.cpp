#include <SDL.h>

#include <Engine/Engine.h>

#include "MainWindow.h"
#include "InterfaceSDL.h"

SEMainWindow::SEMainWindow()
{
    /* Set default values */
    x = SEInterfaceSDL::posWinCentered();
    y = SEInterfaceSDL::posWinCentered();
    api = SE_WINDOW_API_OPENGL;
    mode = SE_WINDOW_MODE_WINDOWED;
    resizable = FALSE;
    status = SE_WINDOW_STATUS_NORMAL;
}

SEMainWindow::~SEMainWindow()
{

}

// close the main application window
void SEMainWindow::close()
{
  // if window exists
  if( pWindow!=NULL) {
    // destroy it
    SDL_DestroyWindow((SDL_Window *) pWindow);
    pWindow = NULL;
  }
}

void SEMainWindow::open()
{
    pWindow = SEInterfaceSDL::createWindow(title, x, y, w, h, api, mode, resizable, status);

    if( pWindow == NULL) FatalError(TRANS("Cannot open main window!"));
    
    SE_UpdateWindowHandle(pWindow);
}