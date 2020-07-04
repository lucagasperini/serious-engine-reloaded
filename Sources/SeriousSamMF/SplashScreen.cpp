#include <SDL_shape.h>

#include "SplashScreen.h"

SESplashScreen::SESplashScreen()
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
}

SESplashScreen::~SESplashScreen()
{
    hide();
}

void SESplashScreen::hide()
{
  if (window) { SDL_DestroyWindow(window); window = NULL; }
  if (renderer) { SDL_DestroyRenderer(renderer); renderer = NULL; }
  if (texture) { SDL_DestroyTexture(texture); texture = NULL; }
}

int SESplashScreen::show()
{
  SDL_Surface *bmp = SDL_LoadBMP(str_bitmap_file);
  
  if (!bmp) return FALSE;

  // RAKE!: commented out as its post SDL2.0.4 |SDL_WINDOW_SKIP_TASKBAR);
  window = SDL_CreateWindow(str_wintitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, bmp->w, bmp->h, SDL_WINDOW_BORDERLESS); 
  
  if(!window) return FALSE;

  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) return FALSE;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  texture = SDL_CreateTextureFromSurface(renderer, bmp);

  if (!texture) {
    hide();
    return FALSE;
  }
  
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  return TRUE;
}
