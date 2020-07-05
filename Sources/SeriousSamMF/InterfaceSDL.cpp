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

HWND SEInterfaceSDL::createWindow(const CTString &title, PIX x, PIX y, PIX w, PIX h, GfxAPIType api, INDEX mode, BOOL resizable, INDEX status)
{
    UINT flags = 0;
    /* FIXME: Make sense to create a separated function for handle this process */
    /* NOTE: This can be a switch-case statement*/
    /* FIXME: Handle GAT_NONE and other definitions */
    if(api == GfxAPIType::GAT_OGL)
        flags = SDL_WINDOW_OPENGL;
    else if(api == GfxAPIType::GAT_VULKAN)
        flags = SDL_WINDOW_VULKAN;

    if(mode == SE_WINDOW_MODE_FULLSCREEN)
        flags = flags | SDL_WINDOW_FULLSCREEN;
    else if(mode == SE_WINDOW_MODE_FULLSCREEN_DESKTOP)
        flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (mode == SE_WINDOW_MODE_BORDERLESS)
        flags = flags | SDL_WINDOW_BORDERLESS;
    else if (mode == SE_WINDOW_MODE_HIDDEN)
        flags = flags | SDL_WINDOW_HIDDEN;
    /* else SE_WINDOW_MODE_WINDOWED */

    if(resizable)
        flags = flags | SDL_WINDOW_RESIZABLE;
    
    if(status == SE_WINDOW_STATUS_MINIMIZED)
        flags = flags | SDL_WINDOW_MINIMIZED;
    else if (status == SE_WINDOW_STATUS_MAXIMIZED)
        flags = flags | SDL_WINDOW_MAXIMIZED;

    return SDL_CreateWindow(title, x, y, w, h, flags);
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