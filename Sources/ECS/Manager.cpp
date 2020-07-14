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

#include "Manager.h"

ULONG ECSManager::s_entity_counter = 0;

ECSManager::ECSManager()
{
    entities = new CDynamicContainer<SEEntity>;
    position_system = new PositionSystem;
    render_system = new RenderSystem;
    input_system = new InputSystem;
    control_system = new ControlSystem;
}

ECSManager::~ECSManager()
{
}

void ECSManager::addEntity(SEEntity* _entity)
{
    _entity->id = s_entity_counter++;
    entities->Add(_entity);
}

void ECSManager::init()
{
    FOREACHINDYNAMICCONTAINER(*entities, SEEntity, entity)
    {
        position_system->init(entity);
        input_system->init(entity);
        render_system->init(entity);
        control_system->init(entity);
    }
    position_system->postinit();
}

void ECSManager::update()
{
    input_system->preupdate();
    render_system->preupdate();
    FOREACHINDYNAMICCONTAINER(*entities, SEEntity, entity)
    {
        input_system->update(entity);
        position_system->update(entity);
        render_system->update(entity);
        control_system->update(entity);
    }
    render_system->postupdate();
    position_system->postupdate();
    input_system->postupdate();
}