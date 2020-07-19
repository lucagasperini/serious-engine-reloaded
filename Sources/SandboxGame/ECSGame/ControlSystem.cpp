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

#include "ControlSystem.h"
#include <ECS/Manager.h>

extern void g_resolution_fullscreen();
extern void g_resolution_change(UINT w, UINT h);
extern BOOL g_dbg_draw_border;
extern BOOL g_dbg_draw_id;
extern BOOL g_dbg_draw_position;
extern BOOL g_dbg_draw_fps;
extern BOOL g_dbg_draw_cursor;
extern UINT g_resolution_width;
extern UINT g_resolution_height;
extern BOOL g_game_started;

void ControlSystem::update(SEEntity* entity)
{
    component_keyboard* keyboard = dynamic_cast<component_keyboard*>((SEEntity*)entity);
    component_keybind* keybind = dynamic_cast<component_keybind*>((SEEntity*)entity);
    component_action* action = dynamic_cast<component_action*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    component_mouseclick* mouseclick = dynamic_cast<component_mouseclick*>((SEEntity*)entity);
    component_mousedelta* mousedelta = dynamic_cast<component_mousedelta*>((SEEntity*)entity);
    component_camera* camera = dynamic_cast<component_camera*>((SEEntity*)entity);

    if (keybind)
        updateGame(keybind);
    if (mouseclick && mousefocus && action)
        updateButton(action, mousefocus, mouseclick);
    if (keyboard && action)
        updateKeyboard(action, keyboard);
    if (camera && mousedelta && keybind)
        updateCamera(camera, mousedelta, keybind);
}

void ControlSystem::updateButton(component_action* _action,
    component_mousefocus* _mousefocus,
    component_mouseclick* _mouseclick)
{
    if (_mousefocus->mf_focus && _mouseclick->mc_button == SDL_BUTTON_LEFT)
        if (_action)
            _action->sea_action();
}

void ControlSystem::updateKeyboard(component_action* _action,
    component_keyboard* _keyboard)
{
    if (_keyboard->kc_key == _keyboard->kc_listen_key)
        if (_action)
            _action->sea_action();
}

void ControlSystem::updateCamera(component_camera* _camera,
    component_mousedelta* _mousedelta,
    component_keybind* _keybind)
{
    switch (_keybind->kb_current) {
    case SE_KEYBIND_CAMERA_RESET:
        //TODO: I dont like world_start_position as global, maybe find a workaround?
        //_camera->cam_pos = world_start_position;
        ECSManager::removeEntity(8);
        break;
    case SE_KEYBIND_CAMERA_RIGHT:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1) + _camera->cam_speed,
            _camera->cam_pos(2),
            _camera->cam_pos(3));
        break;
    case SE_KEYBIND_CAMERA_LEFT:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1) - _camera->cam_speed,
            _camera->cam_pos(2),
            _camera->cam_pos(3));
        break;
    case SE_KEYBIND_CAMERA_FORWARD:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2) + _camera->cam_speed,
            _camera->cam_pos(3));
        break;
    case SE_KEYBIND_CAMERA_BACK:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2) - _camera->cam_speed,
            _camera->cam_pos(3));
        break;
    case SE_KEYBIND_CAMERA_UP:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2),
            _camera->cam_pos(3) + _camera->cam_speed);
        break;
    case SE_KEYBIND_CAMERA_DOWN:
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2),
            _camera->cam_pos(3) - _camera->cam_speed);
        break;
    }
    FLOAT x = _camera->cam_rot(1);
    FLOAT y = _camera->cam_rot(2);
    if (_mousedelta->md_cursor.x) {
        FLOAT ax = _mousedelta->md_cursor.x;
        printf("Camera AX: %f\n", ax);
        x += ax / g_resolution_width * 360;
        printf("Camera Final AX: %f\n", x);
    }
    if (_mousedelta->md_cursor.y) {
        FLOAT ay = _mousedelta->md_cursor.y;
        printf("Camera AY: %f\n", ay);
        y += ay / g_resolution_height * 360;
        printf("Camera Final AY: %f\n", y);
    }

    //if (x > 360.0f)
    //    x -= 360.0f;
    //if (y > 360.0f)
    //    y -= 360.0f;

    //if (x || y)
    _camera->cam_rot = ANGLE3D(x, y, 0.0f);
}

void ControlSystem::updateGame(component_keybind* _keybind)
{
    switch (_keybind->kb_current) {
    case SE_KEYBIND_EXIT:
        g_game_started = FALSE;
        break;
    case SE_KEYBIND_FULLSCREEN:
        g_resolution_fullscreen();
        break;
    case SE_KEYBIND_RESOLUTION_VGA:
        g_resolution_change(640, 480);
        break;
    case SE_KEYBIND_RESOLUTION_SVGA:
        g_resolution_change(800, 600);
        break;
    case SE_KEYBIND_RESOLUTION_WXGA:
        g_resolution_change(1280, 720);
        break;
    case SE_KEYBIND_RESOLUTION_HD:
        g_resolution_change(1920, 1080);
        break;
    case SE_KEYBIND_DEBUG_BORDER:
        g_dbg_draw_border = !g_dbg_draw_border;
        break;
    case SE_KEYBIND_DEBUG_ENTITYID:
        g_dbg_draw_id = !g_dbg_draw_id;
        break;
    case SE_KEYBIND_DEBUG_POSITION:
        g_dbg_draw_position = !g_dbg_draw_position;
        break;
    case SE_KEYBIND_DEBUG_FPS:
        g_dbg_draw_fps = !g_dbg_draw_fps;
        break;
    case SE_KEYBIND_DEBUG_CURSOR:
        g_dbg_draw_cursor = !g_dbg_draw_cursor;
        break;
    }
}