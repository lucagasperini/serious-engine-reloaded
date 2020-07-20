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

#ifndef SER_SBGAME_POSITIONSYSTEM_H
#define SER_SBGAME_POSITIONSYSTEM_H

#include "Entity.h"
#include <ECS/System.h>

class PositionSystem : SESystem {
public:
    void preinit() override {}
    void init(SEEntity* entity) override {}
    void postinit() override {}

    void updateScale(component_position* _position);
    void updateAlign(component_position* _position, component_align* _align);

    void update(SEEntity* _entity) override {};
    void trigger(SEEntity* _entity, UINT _event) override;
};

#endif