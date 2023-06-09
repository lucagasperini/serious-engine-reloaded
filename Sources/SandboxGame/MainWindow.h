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

#ifndef SER_SBGAME_MAINWINDOW_H
#define SER_SBGAME_MAINWINDOW_H

#include <Engine/Base/Types.h>
#include <Engine/Graphics/GfxLibrary.h>

class MainWindow {
private:
    CTString title;
    void* window;
    /* List of Gfx API for Serious Engine:
    GAT_NONE : no gfx API (gfx functions are disabled)
    GAT_OGL : OpenGL
    GAT_D3D : Direct3D
    GAT_VULKAN : Vulkan    FIXME: Actually this engine doesn't support Vulkan, btw adding this for future purpose 
    GAT_CURRENT : Current API  FIXME: I dunno what is it
    */
    GfxAPIType api;
    /*  List of SDL flags:
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
    ULONG flags;
    /* FIXME: what is adapter? it's called on SetDisplayMode() function on create method */
    INDEX adapter;

    /* List of Display Depth for Serious Engine:
    DD_NODEPTH : No depth
    DD_DEFAULT : A default depth FIXME: I dunno what is it
    DD_16BIT : A 16 BIT depth
    DD_32BIT : A 32 BIT depth
    DD_24BIT : A 24 BIT depth with Z-buffer
    */
    DisplayDepth depth;

    CDrawPort* dp;
    CViewPort* vp;

public:
    MainWindow();
    ~MainWindow();

    inline void setTitle(const CTString& _title) { title = _title; }
    inline void setAPI(const GfxAPIType& _api) { api = _api; }
    inline void setDepth(const DisplayDepth& _depth) { depth = _depth; }

    inline CTString getTitle() { return title; }
    inline GfxAPIType getAPI() { return api; }
    inline DisplayDepth getDepth() { return depth; }

    inline CDrawPort* getDrawPort() { return dp; }
    inline CViewPort* getViewPort() { return vp; }

    void init();
    void destroy();
};

#endif