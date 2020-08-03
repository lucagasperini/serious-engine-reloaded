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
    EC_DEBUG_GRABMOUSE = 0xD5,
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

enum SettingCode {
    SC_DEBUG_BORDER = 0xD0,
    SC_DEBUG_ENTITYID = 0xD1,
    SC_DEBUG_POSITION = 0xD2,
    SC_DEBUG_FPS = 0xD3,
    SC_DEBUG_CURSOR = 0xD4,
    SC_DEBUG_GRABMOUSE = 0xD5,
    SC_FULLSCREEN = 0xF0,
    SC_RESOLUTION = 0xF1,
    SC_VIRTUAL_RESOLUTION = 0xF2
};

struct ComponentPosition : component {
    UINT pos_x = 0;
    UINT pos_y = 0;
    UINT pos_w = 0;
    UINT pos_h = 0;
};

struct ComponentTexture : component {
    char* tex_file = NULL;
};

struct ComponentVelocity {
    FLOAT3D velocity = FLOAT3D(0.0f, 0.0f, 0.0f);
};

struct ComponentCamera : component {
    BOOL cam_on = TRUE;
    FLOAT cam_speed = 0.5f;
    FLOAT3D cam_pos = FLOAT3D(0.0f, 0.0f, 0.0f);
    ANGLE3D cam_rot = ANGLE3D(0.0f, 0.0f, 0.0f);
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
    //CFontData fontdata;
    COLOR color = 0xff;
    COLOR color_focus = 0xff;
    char* text = NULL;
    EventCode onclick = EC_NULL;
};

struct ComponentCursor : component {
    char* texture = NULL;
    UINT x = 0;
    UINT y = 0;
    UINT w = 0;
    UINT h = 0;
};

#endif