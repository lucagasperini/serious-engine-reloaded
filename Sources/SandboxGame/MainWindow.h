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

#include <Engine/Base/CTString.h>
#include <Engine/Graphics/DrawPort.h> /* CDrawPort */
#include <Engine/Graphics/ViewPort.h> /* CViewPort */

class SEMainWindow {
private:
    CTString title;
    HWND pWindow;
    PIX x, y, w, h;
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

    // TODO: Make sense use this a extern, not as member with dedicated method?
    CDrawPort* dp;
    // TODO: Make sense use this a extern, not as member with dedicated method?
    CViewPort* vp;

public:
    SEMainWindow();
    ~SEMainWindow();

    inline void setTitle(const CTString& _title) { title = _title; }
    inline void setX(UINT _x) { x = _x; }
    inline void setY(UINT _y) { y = _y; }
    inline void setW(UINT _w) { w = _w; }
    inline void setH(UINT _h) { h = _h; }
    inline void setAPI(GfxAPIType _api) { api = _api; }
    inline void setFlags(INDEX _flags) { flags = _flags; }
    inline void setAdapter(INDEX _adapter) { adapter = _adapter; }
    inline void setDepth(DisplayDepth _depth) { depth = _depth; }
    inline void setDrawPort(const CDrawPort& _dp) { dp = new CDrawPort(_dp); }
    inline void setViewPort(const CViewPort& _vp) { vp = new CViewPort(_vp); }

    inline CTString getTitle() const { return title; }
    inline UINT getX() const { return x; }
    inline UINT getY() const { return y; }
    inline UINT getW() const { return w; }
    inline UINT getH() const { return h; }
    inline INDEX getAPI() const { return api; }
    inline INDEX getFlags() const { return flags; }
    inline INDEX getAdapter() const { return adapter; }
    inline DisplayDepth getDepth() const { return depth; }
    inline CDrawPort* getDrawPort() const { return dp; }
    inline CViewPort* getViewPort() const { return vp; }

    BOOL isIconic();
    BOOL create();
    void destroy();
};

#endif