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
            Manager::quitGame();
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

    if (SER_GET_EVENT(EC_EXIT)) {
        Manager::quitGame();
    }
    if (SER_GET_EVENT(EC_DEBUG_BORDER)) {
        SER_GET_SETTING_ARG(arg, BOOL, SC_DEBUG_BORDER);
        SER_ADD_SETTING(SC_DEBUG_BORDER, new BOOL(!(*arg)), BOOL);
        SER_REMOVE_EVENT(EC_DEBUG_BORDER);
    }
    if (SER_GET_EVENT(EC_DEBUG_POSITION)) {
        SER_GET_SETTING_ARG(arg, BOOL, SC_DEBUG_POSITION);
        SER_ADD_SETTING(SC_DEBUG_POSITION, new BOOL(!(*arg)), BOOL);
        SER_REMOVE_EVENT(EC_DEBUG_POSITION);
    }
    if (SER_GET_EVENT(EC_DEBUG_ENTITYID)) {
        SER_GET_SETTING_ARG(arg, BOOL, SC_DEBUG_ENTITYID);
        SER_ADD_SETTING(SC_DEBUG_ENTITYID, new BOOL(!(*arg)), BOOL);
        SER_REMOVE_EVENT(EC_DEBUG_ENTITYID);
    }
    if (SER_GET_EVENT(EC_DEBUG_FPS)) {
        SER_GET_SETTING_ARG(arg, BOOL, SC_DEBUG_FPS);
        SER_ADD_SETTING(SC_DEBUG_FPS, new BOOL(!(*arg)), BOOL);
        SER_REMOVE_EVENT(EC_DEBUG_FPS);
    }
}
