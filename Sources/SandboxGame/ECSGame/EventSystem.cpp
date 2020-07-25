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

    if (x != old_x || y != old_y) {
        delta_x = old_x - x;
        delta_y = old_y - y;
        event_parameter_mouse[0] = x;
        event_parameter_mouse[1] = y;
        event_parameter_mouse[2] = delta_x;
        event_parameter_mouse[3] = delta_y;
        SER_ADD_EVENT_ARRAY(SER_EVENT_MOUSE_MOVE, event_parameter_mouse, int, 4);
    }

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_started = FALSE;
            g_window_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym) {
            for (UINT i = 0; i < keybind_counter; i++)
                if (a_keybind[i].key == event.key.keysym.sym) {
                    Manager::getEventManager()->add(a_keybind[i].code, a_keybind[i].arg, a_keybind[i].size);
                }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            event_parameter_mouse_click[0] = x;
            event_parameter_mouse_click[1] = y;
            event_parameter_mouse_click[2] = event.button.button;
            SER_ADD_EVENT_ARRAY(SER_EVENT_MOUSE_BUTTON, event_parameter_mouse_click, int, 3);
        }
    }
}

void EventSystem::update(Entity* _entity)
{
    System::update(_entity);

    SER_GET_COMPONENT(cursor, ComponentCursor, _entity);
    if (cursor)
        updateCursor(cursor);

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
        SER_ADD_EVENT(SER_EVENT_BUTTON_ONFOCUS, &this_entity->id, ULONG);
    }
}

void EventSystem::updateCursor(ComponentCursor* _cursor)
{
    //This is safe if none can write on cursor when this event exist.
    if (SER_GET_EVENT_ARG(arg, int, SER_EVENT_MOUSE_MOVE)) {
        _cursor->x = arg[0];
        _cursor->y = arg[1];
    }
}