// Copyright (C) 2020 Luca Gasperini <luca.gasperini@xsoftware.it>
//
// This file is part of Serious Engine Reloaded.
//
// Serious Engine Reloaded is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Serious Engine Reloaded is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Serious Engine Reloaded.  If not, see <http://www.gnu.org/licenses/>.

#include <SDL_shape.h>

#include "SplashScreen.h"

SESplashScreen::SESplashScreen()
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
}

SESplashScreen::~SESplashScreen()
{
    hide();
}

void SESplashScreen::hide()
{
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

BOOL SESplashScreen::show()
{
    SDL_Surface* bmp = SDL_LoadBMP(bitmap);

    if (!bmp)
        return FALSE;

    SDL_SetColorKey(bmp, SDL_RLEACCEL, SDL_MapRGB(bmp->format, 0, 0xff, 0));

    window = SDL_CreateShapedWindow("", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, bmp->w, bmp->h, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

    if (!window)
        return FALSE;

    SDL_WindowShapeMode mode;
    SDL_zero(mode);
    mode.mode = ShapeModeColorKey;
    SDL_Color colorkey = { 0x0, 0xFF, 0x0, 0x0 };
    mode.parameters.colorKey = colorkey;
    if (SDL_SetWindowShape(window, bmp, &mode) != 0) {
        SDL_DestroyWindow(window);
        window = NULL;
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
        return FALSE;

    texture = SDL_CreateTextureFromSurface(renderer, bmp);

    /* get rid of surface */
    SDL_FreeSurface(bmp);

    if (!texture) {
        hide();
        return FALSE;
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    return TRUE;
}
