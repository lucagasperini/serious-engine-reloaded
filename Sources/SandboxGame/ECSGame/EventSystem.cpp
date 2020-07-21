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
        ECSManager::addEvent(SER_EVENT_MOUSE_MOVE, event_parameter_mouse);
    }

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_started = FALSE;
            g_window_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN) {
            for (UINT i = 0; i < SER_KEYBIND_MAX; i++) {
                if (a_keybind[i].key == event.key.keysym.sym && event.key.keysym.sym != 0) {
                    ECSManager::addEvent(a_keybind[i].event);
                }
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            event_parameter_mouse_click[0] = x;
            event_parameter_mouse_click[1] = y;
            event_parameter_mouse_click[2] = event.button.button;
            ECSManager::addEvent(SER_EVENT_MOUSE_BUTTON, event_parameter_mouse_click);
        }
    }
}

void EventSystem::trigger(SEEntity* _entity, SEEvent* _event)
{
    component_window* window = dynamic_cast<component_window*>((SEEntity*)_entity);
    if (window) {
        eventWindow(window, _event);
    }
    component_mouse* mouse = dynamic_cast<component_mouse*>((SEEntity*)_entity);
    if (mouse)
        eventMouse(mouse, _event);
}

void EventSystem::eventMouse(component_mouse* _mouse, SEEvent* _event)
{
    if (_event->code == SER_EVENT_MOUSE_MOVE) {

        int* mouse_status = (int*)_event->parameter;

        _mouse->mouse_x = mouse_status[0];
        _mouse->mouse_y = mouse_status[1];
        _mouse->mouse_delta_x = mouse_status[2];
        _mouse->mouse_delta_y = mouse_status[3];
    }
    if (_event->code == SER_EVENT_MOUSE_BUTTON) {

        int* mouse_status = (int*)_event->parameter;

        _mouse->mouse_x = mouse_status[0];
        _mouse->mouse_y = mouse_status[1];
        _mouse->mouse_button = mouse_status[2];
    }
}

void EventSystem::eventWindow(component_window* _window, SEEvent* _event)
{
    if (_event->code == SER_EVENT_FULLSCREEN_CHANGE) {
        if (_window->win_flags & SDL_WINDOW_FULLSCREEN)
            _window->win_flags = _window->win_flags ^ SDL_WINDOW_FULLSCREEN;
        else
            _window->win_flags = _window->win_flags | SDL_WINDOW_FULLSCREEN;
        g_game_started = FALSE;
    }
    if (_event->code == SER_EVENT_RESOLUTION_CHANGE) {
        UINT* tmp_parameter = (UINT*)_event->parameter;
        if (tmp_parameter != NULL && tmp_parameter[0] != 0 && tmp_parameter[1] != 0) {
            if (g_resolution_width != tmp_parameter[0] || g_resolution_height != tmp_parameter[1]) {
                g_virtual_resolution_width = g_resolution_width;
                g_virtual_resolution_height = g_resolution_height;
                g_resolution_width = tmp_parameter[0];
                g_resolution_height = tmp_parameter[1];
                g_game_started = FALSE;
            }
        }
    }
}