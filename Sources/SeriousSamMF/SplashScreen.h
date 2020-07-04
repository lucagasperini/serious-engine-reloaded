#ifndef SSMF_SPLASHSCREEN_H
#define SSMF_SPLASHSCREEN_H

#include <SDL.h>

#include <Engine/Base/CTString.h>

class SESplashScreen 
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    CTString str_bitmap_file;
    CTString str_wintitle;
public:
    SESplashScreen();
    ~SESplashScreen();
    
    void setBitmapFile(const CTString &_str_bitmap_file) { str_bitmap_file = _str_bitmap_file; }
    void setWinTitle(const CTString &_str_wintitle) { str_wintitle = _str_wintitle; }

    CTString getWinTitle() { return str_wintitle; }
    CTString getBitmapFile() { return str_wintitle; }

    void hide();
    int show();
};

#endif