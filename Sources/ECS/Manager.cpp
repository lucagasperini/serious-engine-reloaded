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
#if DEBUG_ENTITY_FILE == 1
std::mutex ECSManager::mutex_debug;
#endif
std::mutex ECSManager::mutex_preupdate;
std::mutex ECSManager::mutex_preinit;
std::mutex ECSManager::mutex_postinit;
std::mutex ECSManager::mutex_init;
std::mutex ECSManager::mutex_init_counter;
std::mutex ECSManager::mutex_postupdate;
std::condition_variable ECSManager::cv_init;
ULONG ECSManager::number_init = 0;
BOOL ECSManager::wait_init_secure = TRUE;
BYTE** ECSManager::a_thread_memory = NULL;
ULONG ECSManager::thread_number = 0;

std::condition_variable ECSManager::cv_update;
ULONG ECSManager::number_update = 0;
std::mutex ECSManager::mutex_update;
BOOL ECSManager::wait_update_secure = TRUE;

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

    ULONG* obj_size_ptr = (ULONG*)mem_iter;
    mem_iter += sizeof(ULONG);

    SEEntity* return_ptr = (SEEntity*)mem_iter;
    mem_iter += *obj_size_ptr;

    return return_ptr;
}

SEEntity* ECSManager::getEntity(BYTE*& _iter)
{
    if (_iter >= mem_alloc) {
        _iter = a_entity;
        return NULL;
    }

    ULONG* obj_size_ptr = (ULONG*)_iter;
    _iter += sizeof(ULONG);

    SEEntity* return_ptr = (SEEntity*)_iter;
    _iter += *obj_size_ptr;

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

void ECSManager::splitThreadMemory()
{
    if (thread_number <= 0)
        return;

    BYTE* tmp_ptr = a_entity;
    ULONG entity_thread = entity_counter / thread_number;

    a_thread_memory[0] = a_entity;
    for (ULONG n_thread = 1; n_thread < thread_number; n_thread++) {
        for (ULONG n_entity = 0; n_entity < entity_thread; n_entity++) {
            getEntity(tmp_ptr);
        }
        a_thread_memory[n_thread] = tmp_ptr;
    }
}

void ECSManager::run()
{
    if (thread_number > 0) {

        ULONG entity_thread = entity_counter / thread_number;

        for (ULONG i = 0; i < thread_number; i++) {
            if (i == thread_number - 1)
                entity_thread = entity_counter - (entity_thread * (thread_number - 1));

            a_thread[i] = std::thread(runThread, a_thread_memory[i], entity_thread);
        }

    } else {
        runThread(a_entity, entity_counter);
    }
}

void ECSManager::quit()
{
    for (ULONG i = 0; i < thread_number; i++)
        a_thread[i].join();
}

void ECSManager::init(BYTE* _start_ptr, ULONG _number)
{
    SEEntity* tmp_ptr = NULL;
    for (ULONG n_entity = 0; n_entity < _number; n_entity++) {
        tmp_ptr = getEntity(_start_ptr);
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->init(tmp_ptr);
        }
    }
}

void ECSManager::update(BYTE* _start_ptr, ULONG _number)
{
    SEEntity* tmp_ptr = NULL;
    for (ULONG n_entity = 0; n_entity < _number; n_entity++) {
        tmp_ptr = getEntity(_start_ptr);
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->update(tmp_ptr);
        }
    }
}

void ECSManager::runThread(BYTE* _start_ptr, ULONG _number)
{
    {
        std::lock_guard<std::mutex> lg(mutex_preinit);
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->preinit();
        }
    }

    init(_start_ptr, _number);

    {
        std::lock_guard<std::mutex> lg(mutex_postinit);
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->postinit();
        }
    }

    {
        std::lock_guard<std::mutex> lck(mutex_init_counter);

        number_init++;

        if (number_init >= system_counter) {
            wait_init_secure = FALSE;
            number_init = 0;
            cv_init.notify_all();
        }
    }

    {
        std::unique_lock<std::mutex> lck(mutex_init);
        while (wait_init_secure)
            cv_init.wait(lck);
    }

    while (g_game_started) {
        {
            std::lock_guard<std::mutex> lg(mutex_preupdate);
            for (ULONG i = 0; i < system_counter; i++) {
                a_system[i]->preupdate();
            }
        }

        update(_start_ptr, _number);

        {
            std::lock_guard<std::mutex> lg(mutex_postupdate);
            for (ULONG i = 0; i < system_counter; i++) {
                a_system[i]->postupdate();
            }
            number_update++;
        }

        {
            std::unique_lock<std::mutex> lck(mutex_update);
            while (wait_update_secure)
                cv_update.wait(lck);
        }
    }
}