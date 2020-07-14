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

#ifndef SSMF_MANAGER_H
#define SSMF_MANAGER_H

#include "ControlSystem.h"
#include "Entity.h"
#include "InputSystem.h"
#include "PositionSystem.h"
#include "RenderSystem.h"
#include <Engine/Engine.h>

class ECSManager {
private:
    static ULONG entity_counter;

public:
    PositionSystem* position_system = NULL;
    RenderSystem* render_system = NULL;
    InputSystem* input_system = NULL;
    ControlSystem* control_system = NULL;
    CDynamicContainer<SEEntity>* entities = NULL;

    ECSManager();
    ~ECSManager();

    void init();
    void update();

    void addEntity(SEEntity* _entity);
};

#endif