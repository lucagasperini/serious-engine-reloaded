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

#ifndef SER_ECS_MANAGER_H
#define SER_ECS_MANAGER_H

#include "ControlSystem.h"
#include "Entity.h"
#include "InputSystem.h"
#include "PositionSystem.h"
#include "RenderSystem.h"
#include "System.h"
#include <Engine/Engine.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#define SER_ECS_SYSTEM_MAX 64

// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
#define SER_ECS_ENTITY_THREAD_ZERO 0x0000000000000000
// 0000 0000
#define SER_ECS_ENTITY_FLAG_FREE 0x00
// 1000 0000
#define SER_ECS_ENTITY_FLAG_ALLOC 0x80
// 0100 0000
#define SER_ECS_ENTITY_FLAG_READ_ONLY 0x40
// 0000 1000
#define SER_ECS_ENTITY_FLAG_LOCKED 0x08

class ECSManager {
private:
    static ULONG entity_counter;
    // Starting pointer for entity buffer
    static BYTE* a_entity;
    // Maximum space on the buffer in byte
    // Start to 0 and grow by addspace
    // Where: addspace is the size to add in the buffer
    static ULONG mem_entity_max;
    // Pointer for allocations
    // Start at a_entity and jump for sizeof(ULONG) + objsize
    // Where: objsize is the size of an entity
    static BYTE* mem_alloc;
    // Pointer for iterations
    // Start at a_entity and jump for sizeof(ULONG) + objsize
    // Where: objsize is the size of an entity
    static BYTE* mem_iter;

    static ULONG system_counter;

    static SESystem* a_system[SER_ECS_SYSTEM_MAX];

    static std::thread* a_thread;

    static std::condition_variable cv;

    static std::mutex mutex;
    static std::mutex mutex_update;
    static std::mutex mutex_counter;
#if DEBUG_ENTITY_FILE == 1
    static std::mutex mutex_debug;
#endif

    static BOOL secure_wait;

    static ULONG number_init;

    static ULONG number_update;

public:
    static SEEntity* getEntity();
    static SEEntity* getEntity(ULONG _id);

    static SEEntity* getRandomEntity(BYTE*& _ptr, const uint64_t& _thread_flag, const BOOL& _xand);

    static void removeEntity(ULONG _id);
    static void removeEntity(SEEntity* _entity);

    ECSManager();
    ~ECSManager();

    static void grow(ULONG _new);

    static void init(ULONG _system, BOOL _xand);
    static void update(ULONG _system, BOOL _xand);

    static void run();

    static void threadUpdate(ULONG _system);

    static inline void resetEntityIter() { mem_iter = a_entity; }

    static void addEntity(SEEntity* _entity, ULONG _size);
    static void addSystem(SESystem* _system);
};

#endif