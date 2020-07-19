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

#ifndef SER_ECS_COMPONENT_H
#define SER_ECS_COMPONENT_H

#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Base/Types.h>
#include <Engine/Engine.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Math/Projection.h>
#include <Engine/Math/Vector.h>

#define SE_ECS_KEYBIND_MAX 256

#define SE_KEYBIND_NULL 0x00
#define SE_KEYBIND_EXIT 0x01
#define SE_KEYBIND_DEBUG_BORDER 0xD0
#define SE_KEYBIND_DEBUG_ENTITYID 0xD1
#define SE_KEYBIND_DEBUG_POSITION 0xD2
#define SE_KEYBIND_DEBUG_FPS 0xD3
#define SE_KEYBIND_DEBUG_CURSOR 0xD4
#define SE_KEYBIND_CAMERA_RESET 0xC0
#define SE_KEYBIND_CAMERA_RIGHT 0xC1
#define SE_KEYBIND_CAMERA_LEFT 0xC2
#define SE_KEYBIND_CAMERA_FORWARD 0xC3
#define SE_KEYBIND_CAMERA_BACK 0xC4
#define SE_KEYBIND_CAMERA_UP 0xC5
#define SE_KEYBIND_CAMERA_DOWN 0xC6
#define SE_KEYBIND_FULLSCREEN 0xF0
#define SE_KEYBIND_RESOLUTION_VGA 0xF1
#define SE_KEYBIND_RESOLUTION_SVGA 0xF2
#define SE_KEYBIND_RESOLUTION_WXGA 0xF3
#define SE_KEYBIND_RESOLUTION_HD 0xF4

struct component {
};

struct component_position : component {
    UINT pos_x = 0;
    UINT pos_y = 0;
    UINT pos_w = 0;
    UINT pos_h = 0;
};

struct component_texture : component {
    CTextureObject tex_data;
};

struct component_text : component {
    INDEX txt_fontsize = 0;
    INDEX txt_align = 0;
    CFontData txt_fontdata;
    INDEX txt_mode = 0;
    COLOR txt_color = 0xff;
    CTString txt_str;
};

struct component_keyboard : component {
    ULONG kc_key = 0;
    ULONG kc_listen_key = 0;
};

struct component_mouseclick : component {
    ULONG mc_button = 0;
};

struct component_camera : component {
    BOOL cam_on = TRUE;
    FLOAT cam_speed = 1.0f;
    FLOAT3D cam_pos;
    ANGLE3D cam_rot;
    ANGLE cam_fov = 90.0f;
};

struct component_keybind : component {
    ULONG kb_keybind[SE_ECS_KEYBIND_MAX];
    ULONG kb_current = SE_KEYBIND_NULL;
};

struct component_mouse : component {
    POINT mouse_cursor;
};
struct component_mousedelta : component {
    POINT md_cursor;
};

struct component_mousefocus : component {
    BOOL mf_focus = FALSE;
};

struct component_button : component {
    COLOR btn_color2 = 0xff;
};

struct component_action : component {
    void (*sea_action)() = NULL;
};

struct component_align : component {
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

#endif