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

using namespace SER::ECS;

void EventSystem::preupdate()
{
    old_x = x;
    old_y = y;
    // get real cursor position
    SDL_GetMouseState(&x, &y);

    if (x != old_x || y != old_y) {
        delta_x = old_x - x;
        delta_y = old_y - y;
        event_parameter_mouse[0] = x;
        event_parameter_mouse[1] = y;
        event_parameter_mouse[2] = delta_x;
        event_parameter_mouse[3] = delta_y;
        Manager::addEvent(SER_EVENT_MOUSE_MOVE, event_parameter_mouse);
    }

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_started = FALSE;
            g_window_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN) {
            for (UINT i = 0; i < SER_KEYBIND_MAX; i++) {
                if (a_keybind[i].key == event.key.keysym.sym && event.key.keysym.sym != 0) {
                    Manager::addEvent(a_keybind[i].event);
                }
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            event_parameter_mouse_click[0] = x;
            event_parameter_mouse_click[1] = y;
            event_parameter_mouse_click[2] = event.button.button;
            Manager::addEvent(SER_EVENT_MOUSE_BUTTON, event_parameter_mouse_click);
        }
    }
}

void EventSystem::update(SEEntity* _entity)
{
    component_position* position = dynamic_cast<component_position*>((SEEntity*)_entity);
    component_button* button = dynamic_cast<component_button*>((SEEntity*)_entity);
    if (button && position)
        updateButton(position, button);
}

void EventSystem::updateButton(component_position* _position, component_button* _button)
{
    if (_position->pos_x < x && _position->pos_y < y
        && _position->pos_x + _position->pos_w > x
        && _position->pos_y + _position->pos_h > y) {
        Manager::addEvent(SER_EVENT_BUTTON_ONFOCUS, _button);
    }
}