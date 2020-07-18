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
#include <cmath>

#if DEBUG_ENTITY_FILE == 1
#define DEBUG_ECS_FILE(strformat, ...)                    \
    {                                                     \
        std::lock_guard<std::mutex> ul(mutex_debug);      \
        char buffer[1024];                                \
        snprintf(buffer, 1024, strformat, ##__VA_ARGS__); \
        g_logstream.PutString_t(buffer);                  \
    }
#else
#define DEBUG_ECS_FILE(strformat, ...)
#endif

extern BOOL g_game_started;
extern CTFileStream g_logstream;

ULONG ECSManager::entity_counter = 0;
BYTE* ECSManager::a_entity = NULL;
ULONG ECSManager::mem_entity_max = 0;
BYTE* ECSManager::mem_alloc = NULL;
BYTE* ECSManager::mem_iter = NULL;
ULONG ECSManager::system_counter = 0;
SESystem* ECSManager::a_system[SER_ECS_SYSTEM_MAX];
std::thread* ECSManager::a_thread = NULL;
std::mutex ECSManager::mutex;
#if DEBUG_ENTITY_FILE == 1
std::mutex ECSManager::mutex_debug;
#endif
std::mutex ECSManager::mutex_update;
std::mutex ECSManager::mutex_counter;
std::condition_variable ECSManager::cv;
ULONG ECSManager::number_init = 0;
ULONG ECSManager::number_update = 0;
BOOL ECSManager::secure_wait = TRUE;

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

#ifdef DEBUG_ENTITY_FILE
    void* dbg_ptr = mem_alloc;
#endif
    memset(mem_alloc, SER_ECS_ENTITY_THREAD_ZERO, sizeof(uint64_t));
    mem_alloc += sizeof(uint64_t);

    memset(mem_alloc, SER_ECS_ENTITY_FLAG_ALLOC, sizeof(BYTE));
    mem_alloc += sizeof(BYTE);

    memcpy(mem_alloc, &_size, sizeof(ULONG));
    mem_alloc += sizeof(ULONG);

    DEBUG_ECS_FILE("DEBUG: ADD ENTITY ID %u AT %p SIZE %u START AT %p\n", _entity->id, mem_alloc, _size, dbg_ptr);

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

SEEntity* ECSManager::getRandomEntity(BYTE*& _ptr, const uint64_t& _thread_flag, const BOOL& _xand)
{
    // Never use buffer in this function in multithreading mode!
    // If a thread set memory while this has a buffer of previous value and then rewrite this value will lead to undefined behavior

    // If it's requested an out of memory allocated object
    if (_ptr >= mem_alloc) {
        // Reset this pointer to start of the array
        _ptr = a_entity;
        // Return null
        return NULL;
    }

    DEBUG_ECS_FILE("DEBUG: THREAD %i ENTITY REQUEST AT %p WITH ACCESS %u XAND %u\n", (int)log2(_thread_flag), _ptr, *((uint64_t*)_ptr), _xand);

    // Get the thread access list pointer
    uint64_t* thread_access_ptr = (uint64_t*)_ptr;

    // If XAND is 1 and the flag is 1 so this object is access for first time
    // If XAND is 0 and the flag is 0 so this object is access for first time
    if ((_xand && *thread_access_ptr & _thread_flag) || (!_xand && !(*thread_access_ptr & _thread_flag))) {
        BYTE* obj_flag_ptr = _ptr + sizeof(uint64_t);

        // If this object is locked by another thread, skip
        if (!((*obj_flag_ptr) & SER_ECS_ENTITY_FLAG_LOCKED)) {
            // Set this object as locked
            memset(obj_flag_ptr, (*obj_flag_ptr) | SER_ECS_ENTITY_FLAG_LOCKED, sizeof(BYTE));

            // Write new thread access list
            if (_xand)
                // Remove 1 at bit position (2 ^ thread_number)
                *thread_access_ptr = *thread_access_ptr ^ _thread_flag;
            else
                // Add 1 at bit position (2 ^ thread_number)
                *thread_access_ptr = *thread_access_ptr | _thread_flag;

            // Go ahead with the pointer by 9 byte
            _ptr += sizeof(uint64_t);
            _ptr += sizeof(BYTE);

            // Read the size of this object
            ULONG obj_size = (ULONG) * ((ULONG*)_ptr);
            _ptr += sizeof(ULONG);

            // Get the pointer of this object
            SEEntity* return_ptr = (SEEntity*)_ptr;
            // Go at next object
            _ptr += obj_size;

            // Unlock this object
            memset(obj_flag_ptr, (*obj_flag_ptr) ^ SER_ECS_ENTITY_FLAG_LOCKED, sizeof(BYTE));

            DEBUG_ECS_FILE("DEBUG: THREAD %i ENTITY RETURN AT %p SIZE %u\n", (int)log2(_thread_flag), return_ptr, obj_size);
            // Return the pointer of this object
            return return_ptr;
        }
    }
#ifdef DEBUG_ENTITY_FILE
    void* dbg_ptr = _ptr;
#endif
    // Go ahead with the pointer by 9 byte
    _ptr += sizeof(uint64_t);
    _ptr += sizeof(BYTE);
    // Read the size of this object
    ULONG obj_size = (ULONG) * ((ULONG*)_ptr);
    // Go at next object
    _ptr += sizeof(ULONG);
    _ptr += obj_size;

    DEBUG_ECS_FILE("DEBUG: THREAD %i ENTITY SKIP FROM %p AT %p SIZE %u WITH XAND %u\n", (int)log2(_thread_flag), _ptr, dbg_ptr, obj_size, _xand);

    // Return null pointer
    return NULL;
    /*
    if (obj_flag ^ SER_ECS_ENTITY_FLAG_ALLOC) {
        mem_iter += obj_size;
        return getEntity();
    }
*/
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

void ECSManager::run()
{
#ifdef SE_MULTITHREADING_MODE
    a_thread = new std::thread[system_counter];

    for (ULONG i = 0; i < system_counter; i++)
        a_thread[i] = std::thread(threadUpdate, i);

    for (ULONG i = 0; i < system_counter; i++)
        a_thread[i].join();
#else
    BOOL xand = FALSE;
    for (ULONG i = 0; i < system_counter; i++) {
        init(i, xand);
        xand = !xand;
    }
    while (g_game_started) {
        for (ULONG i = 0; i < system_counter; i++) {
            update(i, xand);
            xand = !xand;
        }
    }
#endif
    g_logstream.Close();
}

void ECSManager::init(ULONG _system, BOOL _xand)
{
    ULONG counter = 0;
    BYTE* tmp_ptr = a_entity;
#ifdef SE_MULTITHREADING_MODE
    uint64_t thread_flag = (int64_t)pow(2, _system);
#else
    uint64_t thread_flag = 0x1;
#endif
    while (counter < entity_counter) {
        SEEntity* entity = getRandomEntity(tmp_ptr, thread_flag, _xand);
        if (entity) {
            counter++;
            a_system[_system]->init(entity);
        }
    }
    a_system[_system]->postinit();
}

void ECSManager::update(ULONG _system, BOOL _xand)
{
    ULONG counter = 0;
    BYTE* tmp_ptr = a_entity;
#ifdef SE_MULTITHREADING_MODE
    uint64_t thread_flag = (int64_t)pow(2, _system);
#else
    uint64_t thread_flag = 0x1;
#endif

    a_system[_system]->preupdate();
    while (counter < entity_counter) {
        SEEntity* entity = getRandomEntity(tmp_ptr, thread_flag, _xand);
        if (entity) {
            counter++;
            a_system[_system]->update(entity);
        }
    }
    a_system[_system]->postupdate();
}

void ECSManager::threadUpdate(ULONG _system)
{
    init(_system, FALSE);
    {
        std::lock_guard<std::mutex> ul(mutex);
        number_init++;
    }
    DEBUG_ECS_FILE("DEBUG: THREAD %i END INIT NUMBER %i\n", _system, number_init);
    while (number_init != system_counter) {
    }

    //number_init = 0;
    BOOL xand = FALSE;
    while (g_game_started) {
        DEBUG_ECS_FILE("DEBUG: THREAD %i START UPDATE NUMBER %i\n", _system, number_update);
        xand = !xand;
        update(_system, xand);
        {
            std::lock_guard<std::mutex> lck(mutex_counter);

            if (number_update >= (system_counter - 1)) {
                DEBUG_ECS_FILE("DEBUG: THREAD %u NOTIFY ALL THREAD WITH %i UPDATE\n", _system, number_update);
                secure_wait = FALSE;
                number_update = 0;
                cv.notify_all();
            } else {
                DEBUG_ECS_FILE("DEBUG: THREAD %i END UPDATE NUMBER %i\n", _system, number_update);
                number_update++;
            }
        }

        {
            std::unique_lock<std::mutex> lck(mutex_update);
            while (secure_wait)
                cv.wait(lck);
        }
    }
}