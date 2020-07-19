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

#ifndef SER_SBGAME_ENTITY_H
#define SER_SBGAME_ENTITY_H

#include "Component.h"
#include <ECS/Manager.h>

struct mouse_texture : SEEntity, component_mouse, component_texture {
};

struct MenuImage : SEEntity,
                   component_texture,
                   component_position {
};

struct MenuButton : SEEntity,
                    component_text,
                    component_position,
                    component_align,
                    component_button,
                    //component_mouseclick,
                    //component_mousefocus,
                    component_action {
};
struct GameControl : SEEntity,
                     component_keyboard,
                     component_keybind {
};

struct Camera : SEEntity,
                component_keybind,
                component_mousedelta,
                component_camera,
                component_action {
};

#endif