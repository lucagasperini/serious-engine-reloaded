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
#include "Manager.h"

extern ECSManager* manager;
extern CDrawPort* main_dp;
extern FLOAT3D world_start_position;

void ControlSystem::init(SEEntity* entity)
{
}

void ControlSystem::update(SEEntity* entity)
{
    SEKeyboardComponent* keyboard = dynamic_cast<SEKeyboardComponent*>((SEEntity*)entity);
    SEKeybindComponent* keybind = dynamic_cast<SEKeybindComponent*>((SEEntity*)entity);
    SEActionComponent* action = dynamic_cast<SEActionComponent*>((SEEntity*)entity);
    SEMouseFocusComponent* mousefocus = dynamic_cast<SEMouseFocusComponent*>((SEEntity*)entity);
    SEMouseClickComponent* mouseclick = dynamic_cast<SEMouseClickComponent*>((SEEntity*)entity);
    SEMouseDeltaComponent* mousedelta = dynamic_cast<SEMouseDeltaComponent*>((SEEntity*)entity);
    SECameraComponent* camera = dynamic_cast<SECameraComponent*>((SEEntity*)entity);

    if (keybind)
        control_game(keybind);
    if (mouseclick && mousefocus && action)
        control_button(action, mousefocus, mouseclick);
    if (keyboard && action)
        control_keyboard(action, keyboard);
    if (camera && mousedelta && keybind)
        control_camera(camera, mousedelta, keybind);
}

void ControlSystem::control_button(SEActionComponent* _action, SEMouseFocusComponent* _mousefocus, SEMouseClickComponent* _mouseclick)
{
    if (_mousefocus->mf_focus && _mouseclick->mc_button == SDL_BUTTON_LEFT)
        _action->sea_action();
}

void ControlSystem::control_keyboard(SEActionComponent* _action, SEKeyboardComponent* _keyboard)
{
    if (_keyboard->kc_key == _keyboard->kc_listen_key)
        _action->sea_action();
}

void ControlSystem::control_camera(SECameraComponent* _camera, SEMouseDeltaComponent* _mousedelta, SEKeybindComponent* _keybind)
{
    switch (_keybind->kb_current) {
    case SE_KEYBIND_CAMERA_RESET:
        _camera->cam_pos = world_start_position;
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
    _camera->cam_rot = ANGLE3D(_camera->cam_rot(1) + _mousedelta->md_cursor.x,
        _camera->cam_rot(2) + _mousedelta->md_cursor.y,
        0.0f);
}

extern void resolution_fullscreen();
extern void resolution_change(UINT w, UINT h);
extern BOOL dbg_draw_border;
extern BOOL dbg_draw_id;
extern BOOL dbg_draw_position;
extern BOOL dbg_draw_fps;

void ControlSystem::control_game(SEKeybindComponent* _keybind)
{
    switch (_keybind->kb_current) {
    case SE_KEYBIND_FULLSCREEN:
        resolution_fullscreen();
        break;
    case SE_KEYBIND_RESOLUTION_VGA:
        resolution_change(640, 480);
        break;
    case SE_KEYBIND_RESOLUTION_SVGA:
        resolution_change(800, 600);
        break;
    case SE_KEYBIND_RESOLUTION_WXGA:
        resolution_change(1280, 720);
        break;
    case SE_KEYBIND_RESOLUTION_HD:
        resolution_change(1920, 1080);
        break;
    case SE_KEYBIND_DEBUG_BORDER:
        dbg_draw_border = !dbg_draw_border;
        break;
    case SE_KEYBIND_DEBUG_ENTITYID:
        dbg_draw_id = !dbg_draw_id;
        break;
    case SE_KEYBIND_DEBUG_POSITION:
        dbg_draw_position = !dbg_draw_position;
        break;
    case SE_KEYBIND_DEBUG_FPS:
        dbg_draw_fps = !dbg_draw_fps;
        break;
    }
}