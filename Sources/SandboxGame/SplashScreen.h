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

#ifndef SER_SBGAME_SPLASHSCREEN_H
#define SER_SBGAME_SPLASHSCREEN_H

#include <SDL.h>

#include <Engine/Base/CTString.h>

class SESplashScreen {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    CTString bitmap;

public:
    SESplashScreen();
    ~SESplashScreen();

    inline void setBitmap(const CTString& _bitmap) { bitmap = _bitmap; }

    inline CTString getBitmap() { return bitmap; }

    void hide();
    BOOL show();
};

#endif