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

    static std::condition_variable cv_init;

    static std::mutex mutex_preupdate;
    static std::mutex mutex_preinit;
    static std::mutex mutex_postinit;
    static std::mutex mutex_init;
    static std::mutex mutex_init_counter;
    static std::mutex mutex_postupdate;
    static BOOL wait_init_secure;

#if DEBUG_ENTITY_FILE == 1
    static std::mutex mutex_debug;
#endif

    static ULONG number_init;

    static BYTE** a_thread_memory;

    static ULONG thread_number;

public:
    static std::condition_variable cv_update;
    static ULONG number_update;
    static std::mutex mutex_update;
    static BOOL wait_update_secure;

    static SEEntity* getEntity();
    static SEEntity* getEntity(ULONG _id);
    static SEEntity* getEntity(BYTE*& _iter);

    static void removeEntity(ULONG _id);
    static void removeEntity(SEEntity* _entity);

    ECSManager();
    ~ECSManager();

    static void grow(ULONG _new);

    static void init(BYTE* _start_ptr, ULONG _number);
    static void update(BYTE* _start_ptr, ULONG _number);

    static inline void setThreadNumber(ULONG _thread_number)
    {
        if (a_thread)
            delete[] a_thread;
        if (a_thread_memory)
            delete[] a_thread_memory;

        thread_number = _thread_number;
        a_thread = new std::thread[thread_number];
        a_thread_memory = new BYTE*[thread_number];
    }
    static inline ULONG getThreadNumber() { return thread_number; }

    static void splitThreadMemory();

    static void run();
    static void quit();

    static void runThread(BYTE* _start_ptr, ULONG _number);

    static inline void resetEntityIter() { mem_iter = a_entity; }

    static void addEntity(SEEntity* _entity, ULONG _size);
    static void addSystem(SESystem* _system);
};

#endif