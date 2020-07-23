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

#include "Entity.h"

using namespace SER;

EntityManager::EntityManager()
{
    mem_entity_max = 0;
    counter = 0;
}

EntityManager::~EntityManager()
{
    delete a_entity;
    a_entity = NULL;
}

void EntityManager::grow(ULONG _add)
{
    mem_entity_max += _add;
    a_entity = (BYTE*)malloc(mem_entity_max);
    memset(a_entity, 0, mem_entity_max);

    mem_alloc = a_entity;
}

Entity* EntityManager::get(BYTE*& _iter)
{
    if (_iter >= mem_alloc) {
        _iter = a_entity;
        return NULL;
    }

    if (*_iter ^ SER_ECS_ENTITY_FLAG_ALLOC) {
        _iter += *((ULONG*)_iter + sizeof(BYTE)) + sizeof(BYTE);
        return get(_iter);
    }

    _iter += sizeof(BYTE);

    ULONG* obj_size_ptr = (ULONG*)_iter;
    _iter += sizeof(ULONG);

    Entity* return_ptr = (Entity*)_iter;
    _iter += *obj_size_ptr;

    return return_ptr;
}

Entity* EntityManager::get(ULONG _id)
{
    BYTE* tmp_ptr = a_entity;
    while (Entity* entity = get(tmp_ptr)) {
        if (entity->id == _id)
            return entity;
    }
    return NULL;
}

void EntityManager::add(Entity* _entity, ULONG _size)
{
    _entity->id = counter++;

    memset(mem_alloc, SER_ECS_ENTITY_FLAG_ALLOC, sizeof(BYTE));
    mem_alloc += sizeof(BYTE);

    memcpy(mem_alloc, &_size, sizeof(ULONG));
    mem_alloc += sizeof(ULONG);

    memcpy(mem_alloc, _entity, _size);
    mem_alloc += _size;
}

void EntityManager::remove(ULONG _id)
{
    BYTE* tmp_ptr = a_entity;
    while (Entity* entity = get(tmp_ptr)) {
        if (entity->id == _id) {
            remove(entity);
            return;
        }
    }
}

void EntityManager::remove(Entity* _entity)
{
    BYTE* tmp_ptr = ((BYTE*)_entity) - sizeof(ULONG) - sizeof(BYTE);
    memset(tmp_ptr, SER_ECS_ENTITY_FLAG_FREE, sizeof(BYTE));
}