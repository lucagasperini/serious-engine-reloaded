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

#include "EventSystem.h"

extern UINT g_window_started;
extern UINT g_game_started;
extern UINT g_resolution_width;
extern UINT g_resolution_height;
extern UINT g_virtual_resolution_width;
extern UINT g_virtual_resolution_height;

using namespace SER;

void EventSystem::addKeybind(const EventKeybind& _keybind)
{
    BYTE* mem_tmp = new BYTE[_keybind.size];
    memcpy(mem_tmp, _keybind.arg, _keybind.size);
    a_keybind[keybind_counter++] = EventKeybind { _keybind.code, _keybind.key, mem_tmp, _keybind.size };
}

void EventSystem::preupdate()
{
    old_x = x;
    old_y = y;
    // get real cursor position
    SDL_GetMouseState(&x, &y);

    delta_x = old_x - x;
    delta_y = old_y - y;

    int event_parameter_mouse[4] = { x, y, delta_x, delta_y };

    SER_ADD_EVENT_ARRAY(EC_MOUSE_MOVE, event_parameter_mouse, int, 4);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_started = FALSE;
            g_window_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym) {
            for (UINT i = 0; i < keybind_counter; i++) {
                if (a_keybind[i].key == event.key.keysym.sym) {
                    Manager::getEventManager()->add(a_keybind[i].code, a_keybind[i].arg, a_keybind[i].size);
                }
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int event_parameter_mouse_click[3] = { x, y, event.button.button };
            SER_ADD_EVENT_ARRAY(EC_MOUSE_BUTTON, event_parameter_mouse_click, int, 3);
        }
    }
}

void EventSystem::update(Entity* _entity)
{
    System::update(_entity);

    SER_GET_COMPONENT(cursor, ComponentCursor, _entity);
    if (cursor)
        updateCursor(cursor);

    SER_GET_COMPONENT(camera, ComponentCamera, _entity);
    if (camera)
        updateCamera(camera);

    SER_GET_COMPONENT(position, ComponentPosition, _entity);
    SER_GET_COMPONENT(button, ComponentButton, _entity);

    if (button && position)
        updateButton(position, button);
}

void EventSystem::updateButton(ComponentPosition* _position, ComponentButton* _button)
{
    if (_position->pos_x < x && _position->pos_y < y
        && _position->pos_x + _position->pos_w > x
        && _position->pos_y + _position->pos_h > y) {
        SER_ADD_EVENT(EC_BUTTON_ONFOCUS, &this_entity->id, ULONG);
    }
}

void EventSystem::updateCursor(ComponentCursor* _cursor)
{
    //This is safe if none can write on cursor when this event exist.
    if (SER_GET_EVENT_ARG(arg, int, EC_MOUSE_MOVE)) {
        _cursor->x = arg[0];
        _cursor->y = arg[1];
    }
}

void EventSystem::updateCamera(ComponentCamera* _camera)
{
    if (SER_GET_EVENT(EC_CAMERA_RIGHT)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1) + _camera->cam_speed,
            _camera->cam_pos(2),
            _camera->cam_pos(3));

        SER_REMOVE_EVENT(EC_CAMERA_RIGHT);
    }
    if (SER_GET_EVENT(EC_CAMERA_LEFT)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1) - _camera->cam_speed,
            _camera->cam_pos(2),
            _camera->cam_pos(3));
        SER_REMOVE_EVENT(EC_CAMERA_LEFT);
    }
    if (SER_GET_EVENT(EC_CAMERA_FORWARD)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2),
            _camera->cam_pos(3) + _camera->cam_speed);
        SER_REMOVE_EVENT(EC_CAMERA_FORWARD);
    }
    if (SER_GET_EVENT(EC_CAMERA_BACK)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2),
            _camera->cam_pos(3) - _camera->cam_speed);
        SER_REMOVE_EVENT(EC_CAMERA_BACK);
    }
    if (SER_GET_EVENT(EC_CAMERA_UP)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2),
            _camera->cam_pos(3));
        SER_REMOVE_EVENT(EC_CAMERA_UP);
    }
    if (SER_GET_EVENT(EC_CAMERA_DOWN)) {
        _camera->cam_pos = FLOAT3D(_camera->cam_pos(1),
            _camera->cam_pos(2) - _camera->cam_speed,
            _camera->cam_pos(3));
        SER_REMOVE_EVENT(EC_CAMERA_DOWN);
    }
    if (SER_GET_EVENT_ARG(arg, int, EC_MOUSE_MOVE)) {
        FLOAT x = _camera->cam_rot(1);
        FLOAT y = _camera->cam_rot(2);
        printf("X: %f\nY: %f", (FLOAT)delta_x, (FLOAT)delta_y);
        x += ((FLOAT)delta_x) * _camera->cam_speed / g_resolution_width * 360;
        y += ((FLOAT)delta_y) * _camera->cam_speed / g_resolution_height * 360;

        _camera->cam_rot = ANGLE3D(x, y, 0.0f);
    }
}
