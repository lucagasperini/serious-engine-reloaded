#include <SDL.h>

#include "MainWindow.h"
#include <Engine/Engine.h>

SEMainWindow::SEMainWindow()
{

}

SEMainWindow::~SEMainWindow()
{

}

// close the main application window
void SEMainWindow::close()
{
  // if window exists
  if( hwndMain!=NULL) {
    // destroy it
    SDL_DestroyWindow((SDL_Window *) hwndMain);
    hwndMain = NULL;
  }
}

void SEMainWindow::open(BOOL _fullscreen, PIX pixSizeI, PIX pixSizeJ)
{
    int length = str_title.Length();
    char* title = new char[str_title.Length()];
    strcpy(title, str_title.str_String);
    SDL_snprintf( title, length, TRANS("Serious Sam (FullScreen %dx%d)"), pixSizeI, pixSizeJ);

    if(_fullscreen)
        hwndMain = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixSizeI, pixSizeJ, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    else
        hwndMain = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixSizeI, pixSizeJ, SDL_WINDOW_OPENGL);

    if( hwndMain==NULL) FatalError(TRANS("Cannot open main window!"));
    
    SE_UpdateWindowHandle( hwndMain);
    _pixLastSizeI = pixSizeI;
    _pixLastSizeJ = pixSizeJ;
}