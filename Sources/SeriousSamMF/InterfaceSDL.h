#ifndef SSMF_INTERFACESDL_H
#define SSMF_INTERFACESDL_H

#include <SDL.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/Types.h>

/* FIXME: Those definitions should be on other header for generic purpose */
#define SE_WINDOW_API_OPENGL 0
/* FIXME: Actually this engine doesn't support Vulkan, btw adding this for future purpose */
#define SE_WINDOW_API_VULKAN 1
/* NOTE: You can append DirectX without SDL */

#define SE_WINDOW_MODE_WINDOWED 0
#define SE_WINDOW_MODE_FULLSCREEN 1
#define SE_WINDOW_MODE_FULLSCREEN_DESKTOP 2
#define SE_WINDOW_MODE_BORDERLESS 3
#define SE_WINDOW_MODE_HIDDEN 4

#define SE_WINDOW_STATUS_NORMAL 0
#define SE_WINDOW_STATUS_MINIMIZED 1
#define SE_WINDOW_STATUS_MAXIMIZED 2

/*  
    SDL_WINDOW_FULLSCREEN : fullscreen window
    SDL_WINDOW_FULLSCREEN_DESKTOP: fullscreen window at the current desktop resolution
    SDL_WINDOW_OPENGL : window usable with OpenGL context
    SDL_WINDOW_VULKAN : window usable with a Vulkan instance
    SDL_WINDOW_HIDDEN : window is not visible
    SDL_WINDOW_BORDERLESS : no window decoration
    SDL_WINDOW_RESIZABLE : window can be resized
    SDL_WINDOW_MINIMIZED : window is minimized
    SDL_WINDOW_MAXIMIZED : window is maximized
    SDL_WINDOW_INPUT_GRABBED : window has grabbed input focus
    SDL_WINDOW_ALLOW_HIGHDPI : window should be created in high-DPI mode if supported (>= SDL 2.0.1)
*/

/* NOTE: If you want use different interface, you can create a virtual class like SEInterface and inherit here! */
/* BTW why use another interface when SDL is multi platform? */
class SEInterfaceSDL 
{
private:
public:
    SEInterfaceSDL();
    ~SEInterfaceSDL();

    int init();

    static CTString getError() { return SDL_GetError(); }

    static PIX posWinCentered() { return SDL_WINDOWPOS_CENTERED; }
    static HWND createWindow(const CTString &title, PIX x, PIX y, PIX w, PIX h, INDEX api, INDEX mode, BOOL resizable, INDEX status);
    static void quit();

    static ULONG desktopWidth();
};

#endif