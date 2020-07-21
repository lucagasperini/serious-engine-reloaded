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
extern UINT g_event_current;

ULONG ECSManager::entity_counter = 0;
BYTE* ECSManager::a_entity = NULL;
ULONG ECSManager::mem_entity_max = 0;
BYTE* ECSManager::mem_alloc = NULL;
ULONG ECSManager::system_counter = 0;
SESystem* ECSManager::a_system[SER_ECS_SYSTEM_MAX];
std::thread* ECSManager::a_thread = NULL;
BYTE** ECSManager::a_thread_memory = NULL;
ULONG ECSManager::thread_number = 0;
std::mutex ECSManager::mutex_event;

std::mutex ECSManager::mutex_update;
std::mutex ECSManager::mutex_counter;
std::condition_variable ECSManager::cv_update;
ULONG ECSManager::number_update = 0;
BOOL ECSManager::wait_update = TRUE;

SEEvent ECSManager::a_event[SER_ECS_EVENT_MAX];
ULONG ECSManager::event_number = 0;

SESystem* ECSManager::render_system;

std::mutex ECSManager::mutex_render;
std::condition_variable ECSManager::cv_render;
BOOL ECSManager::wait_render = TRUE;

ULONG ECSManager::loop_status = 0;

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

SEEntity* ECSManager::getEntity(BYTE*& _iter)
{
    if (_iter >= mem_alloc) {
        _iter = a_entity;
        return NULL;
    }

    if (*_iter ^ SER_ECS_ENTITY_FLAG_ALLOC) {
        _iter += *((ULONG*)_iter + sizeof(BYTE)) + sizeof(BYTE);
        return getEntity(_iter);
    }

    _iter += sizeof(BYTE);

    ULONG* obj_size_ptr = (ULONG*)_iter;
    _iter += sizeof(ULONG);

    SEEntity* return_ptr = (SEEntity*)_iter;
    _iter += *obj_size_ptr;

    return return_ptr;
}

SEEntity* ECSManager::getEntity(ULONG _id)
{
    BYTE* tmp_ptr = a_entity;
    while (SEEntity* entity = getEntity(tmp_ptr)) {
        if (entity->id == _id)
            return entity;
    }
    return NULL;
}

void ECSManager::removeEntity(ULONG _id)
{
    BYTE* tmp_ptr = a_entity;
    while (SEEntity* entity = getEntity(tmp_ptr)) {
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

void ECSManager::setThreadNumber(ULONG _thread_number)
{
    if (a_thread)
        delete[] a_thread;
    if (a_thread_memory)
        delete[] a_thread_memory;

    thread_number = _thread_number;
    a_thread = new std::thread[thread_number];
    a_thread_memory = new BYTE*[thread_number];
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
    BYTE* tmp_ptr = a_entity;
    while (SEEntity* entity = getEntity(tmp_ptr)) {
        render_system->init(entity);
    }
    init(a_entity);

    if (thread_number > 0) {

        ULONG entity_thread = entity_counter / thread_number;

        for (ULONG i = 0; i < thread_number; i++) {
            if (i == thread_number - 1)
                entity_thread = entity_counter - (entity_thread * (thread_number - 1));

            a_thread[i] = std::thread(runThread, a_thread_memory[i], entity_thread);
        }
        runThreadRender();
    }
}

void ECSManager::quit()
{
    for (ULONG i = 0; i < thread_number; i++)
        a_thread[i].join();
}

void ECSManager::init(BYTE* _start_ptr)
{
    SEEntity* tmp_ptr = NULL;
    for (ULONG i = 0; i < system_counter; i++) {
        a_system[i]->preinit();
        while (tmp_ptr = getEntity(_start_ptr)) {
            a_system[i]->init(tmp_ptr);
        }
        a_system[i]->postinit();
    }
}

void ECSManager::update(BYTE* _start_ptr, ULONG _number, SEEvent* _event)
{
    SEEntity* tmp_ptr = NULL;

    for (ULONG n_entity = 0; n_entity < _number; n_entity++) {
        tmp_ptr = getEntity(_start_ptr);
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->update(tmp_ptr);
            if (_event)
                a_system[i]->trigger(tmp_ptr, _event);
        }
    }
}

void ECSManager::runThread(BYTE* _start_ptr, ULONG _number)
{
    while (g_game_started) {
        {
            std::unique_lock<std::mutex> lck(mutex_update);
            cv_update.notify_all();
            cv_update.wait(lck, [] { return loop_status >= 0 && loop_status <= thread_number; });
        }

        SEEvent* event = getEvent();
        update(_start_ptr, _number, event);

        {
            std::lock_guard<std::mutex> lck(mutex_counter);
            loop_status++;
            //removeEvent(event->code);
        }
    }
}

void ECSManager::runThreadRender()
{
    while (g_game_started) {
        {
            std::unique_lock<std::mutex> lck(mutex_update);
            cv_update.notify_all();
            cv_update.wait(lck, [] { return loop_status >= thread_number + 1; });
        }
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->postupdate();
        }

        render_system->preupdate();
        SEEvent* event = getEvent();
        BYTE* tmp_ptr = getFirst();
        while (SEEntity* entity = getEntity(tmp_ptr)) {
            render_system->update(entity);
            render_system->trigger(entity, event);
        }
        render_system->postupdate();

        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->preupdate();
        }

        loop_status = 0;
    }
}
SEEvent* ECSManager::getEvent()
{
    if (event_number)
        return a_event;
    return NULL;
}

void ECSManager::addEvent(UINT _code, void* _parameter)
{
    std::lock_guard<std::mutex> lg(mutex_event);
    if (event_number < SER_ECS_EVENT_MAX) {
        a_event[event_number].code = _code;
        a_event[event_number++].parameter = _parameter;
    }
}

void ECSManager::addEvent(const SEEvent& _event)
{
    std::lock_guard<std::mutex> lg(mutex_event);
    if (event_number < SER_ECS_EVENT_MAX)
        a_event[event_number++] = _event;
}

void ECSManager::removeEvent(UINT _event)
{
    if (event_number <= 0)
        return;
    for (ULONG i = 0; i < event_number; i++) {
        if (a_event[i].code == _event) {
            std::lock_guard<std::mutex> lg(mutex_event);
            a_event[i].code = a_event[i + 1].code;
            a_event[i].parameter = a_event[i + 1].parameter;
            event_number--;
        }
    }
}

void ECSManager::removeEvent()
{
    if (event_number > 0) {
        std::lock_guard<std::mutex> lg(mutex_event);
        for (ULONG i = 0; i < event_number; i++)
            a_event[i] = a_event[i + 1];

        event_number--;
    }
}

BOOL ECSManager::searchEvent(UINT _event)
{
    for (ULONG i = 0; i < event_number; i++)
        if (a_event[i].code == _event)
            return TRUE;
    return FALSE;
}