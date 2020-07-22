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

struct main_window : SEEntity, component_window {
};

struct cursor : SEEntity, component_cursor {
};

struct MenuImage : SEEntity,
                   component_texture,
                   component_position {
};

struct MenuButton : SEEntity,
                    component_position,
                    component_align,
                    component_button {
};

struct Camera : SEEntity,
                component_camera {
};

#endif