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

#include "InputSystem.h"

extern BOOL g_game_started;
extern int SE_SDL_InputEventPoll(SDL_Event* event);

void InputSystem::init(SEEntity* entity)
{
}

void InputSystem::preupdate()
{
    // get real cursor position
    GetCursorPos(&cursor);
    if (old_cursor != NULL) {
        deltacursor.x = (old_cursor->x - cursor.x) * sensibility;
        deltacursor.y = (old_cursor->y - cursor.y) * sensibility;
    } else {
        old_cursor = new POINT();
    }
    old_cursor->x = cursor.x;
    old_cursor->y = cursor.y;

    while (SE_SDL_InputEventPoll(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN) {
            key = event.key.keysym.sym;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            button = event.button.button;
        }
    }
}

void InputSystem::postupdate()
{
    key = 0;
    button = 0;
    cursor = POINT();
    deltacursor = POINT();
}

void InputSystem::update(SEEntity* entity)
{
    component_keyboard* keyboard = dynamic_cast<component_keyboard*>((SEEntity*)entity);
    if (keyboard)
        keyboard->kc_listen_key = key;
    component_keybind* keybind = dynamic_cast<component_keybind*>((SEEntity*)entity);
    if (keybind) {
        // Reset current keybind first
        keybind->kb_current = SE_KEYBIND_NULL;
        for (ULONG i = 1; i < SE_ECS_KEYBIND_MAX; i++) {
            if (keybind->kb_keybind[i] == key && key != 0) {
                keybind->kb_current = i;
                break;
            }
        }
    }

    component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    if (position && mousefocus) {
        if (position->pos_x < cursor.x && position->pos_y < cursor.y && position->pos_x + position->pos_w > cursor.x && position->pos_y + position->pos_h > cursor.y) {
            mousefocus->mf_focus = TRUE;
        } else {
            mousefocus->mf_focus = FALSE;
        }
    }

    component_mouseclick* mouseclick = dynamic_cast<component_mouseclick*>((SEEntity*)entity);
    if (mouseclick)
        mouseclick->mc_button = button;

    component_mousedelta* mousedelta = dynamic_cast<component_mousedelta*>((SEEntity*)entity);
    if (mousedelta) {
        mousedelta->md_cursor.x = deltacursor.x;
        mousedelta->md_cursor.y = deltacursor.y;
    }
}