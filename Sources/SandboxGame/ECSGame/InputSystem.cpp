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
extern UINT g_resolution_width;
extern UINT g_resolution_height;
extern UINT g_event_current;
extern POINT g_cursor_position;
extern UINT g_press_key;
extern UINT g_press_button;

void InputSystem::preinit()
{
    deltacursor.x = 0;
    deltacursor.y = 0;
}

void InputSystem::update(SEEntity* entity)
{
    /*
    component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    if (position && mousefocus) {
        if (position->pos_x < g_cursor_position.x
            && position->pos_y < g_cursor_position.y
            && position->pos_x + position->pos_w > g_cursor_position.x
            && position->pos_y + position->pos_h > g_cursor_position.y) {
            mousefocus->mf_focus = TRUE;
        } else {
            mousefocus->mf_focus = FALSE;
        }
    }
*/
}

void InputSystem::trigger(SEEntity* _entity, SEEvent* _event)
{
}