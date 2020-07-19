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

#ifndef SER_SBGAME_CONTROLSYSTEM_H
#define SER_SBGAME_CONTROLSYSTEM_H

#include "Entity.h"
#include <ECS/System.h>

class ControlSystem : SESystem {
public:
    void preinit() override {}
    void init(SEEntity* entity) override {}
    void postinit() override {}

    void preupdate() override {}
    void update(SEEntity* entity) override;
    void postupdate() override {}

    void control_button(component_action* _action, component_mousefocus* _mousefocus, component_mouseclick* _mouseclick);
    void control_keyboard(component_action* _action, component_keyboard* _keyboard);
    void control_camera(component_camera* _camera, component_mousedelta* _mousedelta, component_keybind* _keybind);
    void control_game(component_keybind* _keybind);
};

#endif