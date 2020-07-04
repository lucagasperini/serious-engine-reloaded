#ifndef SSMF_INTERFACESDL_H
#define SSMF_INTERFACESDL_H

#include <SDL.h>

#include <Engine/Base/CTString.h>
#include <Engine/Base/Types.h>

class SEInterfaceSDL 
{
private:
public:
    SEInterfaceSDL();
    ~SEInterfaceSDL();

    int init();

    CTString getError() { return SDL_GetError(); }

    static void quit();

    ULONG desktopWidth();
};

#endif