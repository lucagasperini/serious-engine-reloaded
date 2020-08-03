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

#ifndef SER_ECS_ENTITY_H
#define SER_ECS_ENTITY_H

#include <Engine/Base/Types.h>

// 0000 0000
#define SER_ECS_ENTITY_FLAG_FREE 0x00
// 0000 0001
#define SER_ECS_ENTITY_FLAG_ALLOC 0x01

namespace SER {
struct Entity {
    ULONG id;
    virtual ~Entity() {}
};

struct FastEntityMemory {
    ULONG size;
    char* str;
    void** ptr;
};

class EntityManager {
private:
    ULONG counter;
    // Starting pointer for entity buffer
    BYTE* a_entity;
    // Maximum space on the buffer in byte
    // Start to 0 and grow by addspace
    // Where: addspace is the size to add in the buffer
    ULONG mem_entity_max;
    // Pointer for allocations
    // Start at a_entity and jump for sizeof(ULONG) + objsize
    // Where: objsize is the size of an entity
    BYTE* mem_alloc;

public:
    EntityManager();
    ~EntityManager();

    void grow(ULONG _new);

    Entity* get(ULONG _id);
    Entity* get(BYTE*& _iter);
    inline BYTE* ptr() { return a_entity; }

    void add(Entity* _entity, ULONG _size, FastEntityMemory* _a_fem, ULONG _fem_count);
    inline void add(Entity* _entity, ULONG _size) { add(_entity, _size, NULL, 0); }

    void remove(ULONG _id);
    void remove(Entity* _entity);

    BOOL saveDisk(CTFileName _file);
    BOOL loadDisk(CTFileName _file);

    inline ULONG count() { return counter; }
    inline ULONG sizeMax() { return mem_entity_max; }
    inline ULONG sizeAlloc() { return mem_alloc - a_entity; }
};

}
#endif