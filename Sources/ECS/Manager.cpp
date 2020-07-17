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

ULONG ECSManager::entity_counter = 0;
BYTE* ECSManager::a_entity = NULL;
ULONG ECSManager::mem_entity_max = 0;
BYTE* ECSManager::mem_alloc = NULL;
BYTE* ECSManager::mem_iter = NULL;
ULONG ECSManager::system_counter = 0;
SESystem* ECSManager::a_system[SER_ECS_SYSTEM_MAX];
std::thread* ECSManager::a_thread = NULL;
std::mutex ECSManager::mutex;
ULONG ECSManager::number_init = 0;

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

    memset(mem_alloc, SER_ECS_ENTITY_THREAD_ZERO, sizeof(uint64_t));
    mem_alloc += sizeof(uint64_t);

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

SEEntity* ECSManager::getRandomEntity(BYTE*& _ptr, uint64_t _thread_flag)
{
    if (_ptr > mem_alloc) {
        _ptr = a_entity;
        return NULL;
    }

    uint64_t thread_access = *((uint64_t*)_ptr);
    if (thread_access ^ _thread_flag) {

        BYTE* obj_flag_ptr = _ptr + sizeof(uint64_t);
        BYTE obj_flag = *obj_flag_ptr;

        if (obj_flag ^ SER_ECS_ENTITY_FLAG_LOCKED) {
            memset(obj_flag_ptr, obj_flag | SER_ECS_ENTITY_FLAG_LOCKED, sizeof(BYTE));
            memset(_ptr, thread_access | _thread_flag, sizeof(uint64_t));
            _ptr += sizeof(uint64_t);
            _ptr += sizeof(BYTE);

            ULONG obj_size = (ULONG) * ((ULONG*)_ptr);
            _ptr += sizeof(ULONG);

            SEEntity* return_ptr = (SEEntity*)_ptr;
            _ptr += obj_size;

            memset(obj_flag_ptr, obj_flag ^ SER_ECS_ENTITY_FLAG_LOCKED, sizeof(BYTE));

            return return_ptr;
        }
    }
    _ptr += sizeof(uint64_t);
    _ptr += sizeof(BYTE);
    ULONG obj_size = (ULONG) * ((ULONG*)_ptr);
    _ptr += sizeof(ULONG);
    _ptr += obj_size;

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

void ECSManager::init()
{
    /*
    if (s_thread == NULL)
        s_thread = new std::thread[system_counter];
    for (ULONG i = 0; i < system_counter; i++) {
        s_thread[i] = std::thread(thread_init, i);
        s_thread[i].join();
    }
    */
    for (ULONG i = 0; i < system_counter; i++) {
    }
}

void ECSManager::update()
{
    a_thread = new std::thread[system_counter];

    for (ULONG i = 0; i < system_counter; i++)
        a_thread[i] = std::thread(threadUpdate, i);

    while (g_game_started) {
    }
    for (ULONG i = 0; i < system_counter; i++)
        a_thread[i].join();
}

void ECSManager::threadUpdate(ULONG _system)
{
    a_system[_system]->preinit();
    ULONG counter = 0;
    BYTE* tmp_ptr = a_entity;
    uint64_t thread_flag = (int64_t)pow(2, _system);

    while (counter < entity_counter) {
        SEEntity* entity = getRandomEntity(tmp_ptr, thread_flag);
        if (entity) {
            counter++;
            a_system[_system]->init(entity);
        }
    }
    a_system[_system]->postinit();

    std::unique_lock<std::mutex> ul(mutex);
    number_init++;
    ul.unlock();
    while (number_init != system_counter) {
    }

    //number_init = 0;

    while (g_game_started) {
        a_system[_system]->preupdate();
        counter = 0;
        tmp_ptr = a_entity;
        while (counter < entity_counter) {
            SEEntity* entity = getRandomEntity(tmp_ptr, thread_flag);
            if (entity) {
                counter++;
                a_system[_system]->update(entity);
            }
        }
        a_system[_system]->postupdate();
    }
}