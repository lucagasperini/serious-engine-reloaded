#include "InterfaceSDL.h"

SEInterfaceSDL::SEInterfaceSDL()
{

}

SEInterfaceSDL::~SEInterfaceSDL()
{

}

int SEInterfaceSDL::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
        return FALSE;

    atexit(quit);
    SDL_Init(SDL_INIT_JOYSTICK);  // don't care if this fails.
    return TRUE;
}

// FIXME: Remove copy of this function from Engine/Graphics/SDL/SDLAdapter.cpp 

ULONG SEInterfaceSDL::desktopWidth()
{
    const int dpy = 0;   // !!! FIXME: add a cvar for this?
    SDL_DisplayMode mode;
    const int rc = SDL_GetDesktopDisplayMode(dpy, &mode);
    ASSERT(rc == 0);
    return(mode.w);
}

void SEInterfaceSDL::quit() { 
        SDL_Quit(); 
}