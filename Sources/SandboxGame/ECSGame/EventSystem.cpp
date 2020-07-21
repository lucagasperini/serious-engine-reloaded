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

extern UINT g_game_started;
extern UINT g_resolution_width;
extern UINT g_resolution_height;
extern UINT g_virtual_resolution_width;
extern UINT g_virtual_resolution_height;

void EventSystem::trigger(SEEntity* _entity, SEEvent* _event)
{
    component_window* window = dynamic_cast<component_window*>((SEEntity*)_entity);
    if (window) {
        eventWindow(window, _event);
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
                ECSManager::addEvent(SER_EVENT_SCALE_UI, NULL);
            }
        }
    }
}