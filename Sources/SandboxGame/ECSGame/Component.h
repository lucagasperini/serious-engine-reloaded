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

#ifndef SER_SBGAME_COMPONENT_H
#define SER_SBGAME_COMPONENT_H

#include <ECS/Component.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Base/Types.h>
#include <Engine/Engine.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Math/Projection.h>
#include <Engine/Math/Vector.h>

enum EventCode {
    EC_NULL = 0x00,
    EC_EXIT = 0x01,
    EC_BUTTON_ONFOCUS = 0x80,
    EC_DEBUG_BORDER = 0xD0,
    EC_DEBUG_ENTITYID = 0xD1,
    EC_DEBUG_POSITION = 0xD2,
    EC_DEBUG_FPS = 0xD3,
    EC_DEBUG_CURSOR = 0xD4,
    EC_CAMERA_RESET = 0xC0,
    EC_CAMERA_RIGHT = 0xC1,
    EC_CAMERA_LEFT = 0xC2,
    EC_CAMERA_FORWARD = 0xC3,
    EC_CAMERA_BACK = 0xC4,
    EC_CAMERA_UP = 0xC5,
    EC_CAMERA_DOWN = 0xC6,
    EC_MOUSE_MOVE = 0xE0,
    EC_MOUSE_BUTTON = 0xE1,
    EC_FULLSCREEN_CHANGE = 0xF0,
    EC_RESOLUTION_CHANGE = 0xF1,
};
struct ComponentWindow : component {
    CTString win_title;
    void* win_pointer = NULL;
    /* List of Gfx API for Serious Engine:
    GAT_NONE : no gfx API (gfx functions are disabled)
    GAT_OGL : OpenGL
    GAT_D3D : Direct3D
    GAT_VULKAN : Vulkan    FIXME: Actually this engine doesn't support Vulkan, btw adding this for future purpose 
    GAT_CURRENT : Current API  FIXME: I dunno what is it
    */
    GfxAPIType win_api = GfxAPIType::GAT_OGL;
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
    ULONG win_flags = NULL;
    /* FIXME: what is adapter? it's called on SetDisplayMode() function on create method */
    INDEX win_adapter = 0;

    /* List of Display Depth for Serious Engine:
    DD_NODEPTH : No depth
    DD_DEFAULT : A default depth FIXME: I dunno what is it
    DD_16BIT : A 16 BIT depth
    DD_32BIT : A 32 BIT depth
    DD_24BIT : A 24 BIT depth with Z-buffer
    */
    DisplayDepth win_depth = DisplayDepth::DD_32BIT;
};

struct ComponentPosition : component {
    UINT pos_x = 0;
    UINT pos_y = 0;
    UINT pos_w = 0;
    UINT pos_h = 0;
};

struct ComponentTexture : component {
    CTextureObject tex_data;
};

struct ComponentCamera : component {
    BOOL cam_on = TRUE;
    FLOAT cam_speed = 1.0f;
    FLOAT3D cam_pos;
    ANGLE3D cam_rot;
    ANGLE cam_fov = 90.0f;
};

struct ComponentAlign : component {
    /*
    -2 => Left
    -1 => Center Left
    0  => Center
    1  => Center Right
    2  => Right 
    */
    INDEX align_x = 64;
    /*
    -2 => Bottom
    -1 => Center Bottom
    0  => Center
    1  => Center Top
    2  => Top 
    */
    INDEX align_y = 64;
};

struct ComponentButton : component {
    INDEX fontsize = 0;
    INDEX align = 0;
    CFontData fontdata;
    COLOR color = 0xff;
    COLOR color_focus = 0xff;
    CTString text;
};

struct ComponentCursor : component {
    CTextureObject texture;
    UINT x = 0;
    UINT y = 0;
    UINT w = 0;
    UINT h = 0;
};

#endif