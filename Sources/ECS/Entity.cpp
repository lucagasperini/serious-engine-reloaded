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
    a_entity = NULL;
    mem_alloc = NULL;
    mem_entity_max = 0;
    counter = 0;
}

EntityManager::~EntityManager()
{
    delete[] a_entity;
    a_entity = NULL;
    counter = 0;
    mem_entity_max = 0;
    mem_alloc = NULL;
}

//TODO: Add a runtime grow with memory copy
void EntityManager::grow(ULONG _add)
{
    mem_entity_max += _add;
    a_entity = (BYTE*)malloc(mem_entity_max);
    memset(a_entity, 0, mem_entity_max);

    counter = 0;
    mem_alloc = a_entity;
}

Entity* EntityManager::get(BYTE*& _iter)
{
    if (_iter >= mem_alloc) {
        _iter = a_entity;
        return NULL;
    }

    ULONG* size_block_ptr = (ULONG*)_iter;
    BYTE* flag_ptr = _iter + sizeof(ULONG);

    _iter += *size_block_ptr;

    if (*flag_ptr < 1) {

        return get(_iter);
    }

    ULONG* entity_size_ptr = (ULONG*)(flag_ptr + 1);
    Entity* entity_ptr = (Entity*)(entity_size_ptr + 1);

    return entity_ptr;
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

void EntityManager::add(Entity* _entity, ULONG _size, FastEntityMemory* _a_fem, ULONG _fem_count)
{
    _entity->id = counter++;

    BYTE* start_ptr = mem_alloc;

    ULONG* size_block_ptr = (ULONG*)mem_alloc;
    mem_alloc += sizeof(ULONG);

    BYTE* flag_ptr = mem_alloc;
    mem_alloc += sizeof(BYTE);

    ULONG* size_entity_ptr = (ULONG*)mem_alloc;
    mem_alloc += sizeof(ULONG);

    Entity* entity_ptr = (Entity*)mem_alloc;
    mem_alloc += _size;

    ULONG* a_subobj_size[_fem_count];
    BYTE* a_subobj[_fem_count];

    for (ULONG i = 0; i < _fem_count; i++) {
        a_subobj_size[i] = (ULONG*)mem_alloc;
        mem_alloc += sizeof(ULONG);
        *_a_fem[i].ptr = mem_alloc;
        a_subobj[i] = mem_alloc;
        mem_alloc += _a_fem[i].size;
    }

    ULONG size_block = mem_alloc - start_ptr;
    memcpy(size_block_ptr, &size_block, sizeof(ULONG));

    BYTE flag = _fem_count + SER_ECS_ENTITY_FLAG_ALLOC;
    memset(flag_ptr, flag, sizeof(BYTE));

    memcpy(size_entity_ptr, &_size, sizeof(ULONG));

    memcpy(entity_ptr, _entity, _size);

    for (ULONG i = 0; i < _fem_count; i++) {
        memcpy(a_subobj_size[i], &_a_fem[i].size, sizeof(ULONG));

        memcpy(a_subobj[i], _a_fem[i].str, _a_fem[i].size);
    }
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