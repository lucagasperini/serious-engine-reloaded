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
#include <SER/ECS/System.h>

class PositionSystem : public SER::System {
private:
    UINT scale_x = 0;
    UINT scale_y = 0;

public:
    void preinit() override {}
    void init(SER::Entity* entity) override;
    void postinit() override;

    void initScale(ComponentPosition* _position);
    void initAlign(ComponentPosition* _position, ComponentAlign* _align);

    void preupdate() override {};
    void update(SER::Entity* _entity) override;
    void postupdate() override {};

    void updateCursor(ComponentCursor* _cursor);
    void updateCamera(ComponentCamera* _camera, ComponentVelocity* _velocity);
    void updateButton(ComponentPosition* _position, ComponentButton* _button);
};

#endif