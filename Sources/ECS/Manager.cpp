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

ECSManager::ECSManager()
{

    mem_entity_max = 0;
    entity_counter = 0;
    system_counter = 0;
}

ECSManager::~ECSManager()
{
    //delete a_system;
    //a_system = NULL;
    delete a_entity;
    a_entity = NULL;
}

void ECSManager::addSystem(SESystem* _system)
{
    a_system[system_counter++] = _system;
}

void ECSManager::grow(ULONG _add)
{
    mem_entity_max += _add;
    a_entity = (BYTE*)malloc(mem_entity_max);
    memset(a_entity, 0, mem_entity_max);

    mem_alloc = a_entity;
    mem_iter = a_entity;
}

void ECSManager::addEntity(SEEntity* _entity, ULONG _size)
{
    _entity->id = entity_counter++;

    memset(mem_alloc, SER_ECS_ENTITY_FLAG_ALLOC, sizeof(BYTE));
    mem_alloc += sizeof(BYTE);

    memcpy(mem_alloc, &_size, sizeof(ULONG));
    mem_alloc += sizeof(ULONG);

    memcpy(mem_alloc, _entity, _size);
    mem_alloc += _size;
}

SEEntity* ECSManager::getEntity()
{
    if (mem_iter >= mem_alloc) {
        resetEntityIter();
        return NULL;
    }
    BYTE obj_flag = (*mem_iter);
    mem_iter += sizeof(BYTE);

    ULONG obj_size = (ULONG) * ((ULONG*)mem_iter);
    mem_iter += sizeof(ULONG);

    if (obj_flag ^ SER_ECS_ENTITY_FLAG_ALLOC) {
        mem_iter += obj_size;
        return getEntity();
    }

    SEEntity* return_ptr = (SEEntity*)mem_iter;
    mem_iter += obj_size;
    return return_ptr;
}

SEEntity* ECSManager::getEntity(ULONG _id)
{
    while (SEEntity* entity = getEntity()) {
        if (entity->id == _id)
            return entity;
    }
    return NULL;
}

void ECSManager::removeEntity(ULONG _id)
{
    while (SEEntity* entity = getEntity()) {
        if (entity->id == _id) {
            removeEntity(entity);
            return;
        }
    }
}

void ECSManager::removeEntity(SEEntity* _entity)
{
    BYTE* tmp_ptr = ((BYTE*)_entity) - sizeof(ULONG) - sizeof(BYTE);
    memset(tmp_ptr, SER_ECS_ENTITY_FLAG_FREE, sizeof(BYTE));
}

void ECSManager::init()
{
    for (ULONG i = 0; i < system_counter; i++)
        a_system[i]->preinit();
    while (SEEntity* entity = getEntity()) {
        for (ULONG i = 0; i < system_counter; i++)
            a_system[i]->init(entity);
    }
    for (ULONG i = 0; i < system_counter; i++)
        a_system[i]->postinit();
}

void ECSManager::update()
{
    for (ULONG i = 0; i < system_counter; i++)
        a_system[i]->preupdate();

    while (SEEntity* entity = getEntity()) {
        for (ULONG i = 0; i < system_counter; i++)
            a_system[i]->update(entity);
    }

    for (ULONG i = 0; i < system_counter; i++)
        a_system[i]->postupdate();
}