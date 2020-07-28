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

using namespace SER;

extern BOOL g_game_started;
extern BOOL g_window_started;

ULONG Manager::system_counter = 0;
System* Manager::a_system[SER_ECS_SYSTEM_MAX];
std::thread* Manager::a_thread = NULL;
std::thread Manager::thread_event;
BYTE** Manager::a_thread_memory = NULL;
ULONG Manager::thread_number = 0;

System* Manager::render_system;
System* Manager::event_system;

ULONG Manager::loop_status = 0;
BOOL Manager::is_end_frame = FALSE;

EntityManager* Manager::entity_manager;
EventManager* Manager::event_manager;

Manager::Manager()
{
}

Manager::~Manager()
{
    //delete a_system;
    //a_system = NULL;
}

void Manager::init(ULONG _entity_space, ULONG _event_limit)
{
    entity_manager = new EntityManager;
    entity_manager->grow(_entity_space);
    event_manager = new EventManager;
    event_manager->grow(_event_limit);
    system_counter = 0;
}

void Manager::addSystem(System* _system)
{
    a_system[system_counter++] = _system;
}

void Manager::setThreadNumber(ULONG _thread_number)
{
    if (a_thread)
        delete[] a_thread;
    if (a_thread_memory)
        delete[] a_thread_memory;

    thread_number = _thread_number;
    a_thread = new std::thread[thread_number];
    a_thread_memory = new BYTE*[thread_number];
}

void Manager::splitThreadMemory()
{
    if (thread_number <= 0)
        return;

    BYTE* tmp_ptr = entity_manager->ptr();
    ULONG entity_thread = entity_manager->count() / thread_number;

    a_thread_memory[0] = entity_manager->ptr();
    for (ULONG n_thread = 1; n_thread < thread_number; n_thread++) {
        for (ULONG n_entity = 0; n_entity < entity_thread; n_entity++) {
            entity_manager->get(tmp_ptr);
        }
        a_thread_memory[n_thread] = tmp_ptr;
    }
}

void Manager::run()
{
    BYTE* tmp_ptr = entity_manager->ptr();
    while (Entity* entity = entity_manager->get(tmp_ptr)) {
        render_system->init(entity);
    }

    tmp_ptr = entity_manager->ptr();
    for (ULONG i = 0; i < system_counter; i++) {
        a_system[i]->preinit();
        while (Entity* entity = entity_manager->get(tmp_ptr)) {
            a_system[i]->init(entity);
        }
        a_system[i]->postinit();
    }

    if (thread_number > 0) {
        loop_status = thread_number + 2;
        ULONG entity_thread = entity_manager->count() / thread_number;

        for (ULONG i = 0; i < thread_number; i++) {
            if (i == thread_number - 1)
                entity_thread = entity_manager->count() - (entity_thread * (thread_number - 1));

            a_thread[i] = std::thread(runThread, a_thread_memory[i], entity_thread);
        }
        thread_event = std::thread(runThreadEvent);
        runThreadRender();
    }
}

void Manager::quit()
{
    thread_event.join();
    for (ULONG i = 0; i < thread_number; i++) {
        a_thread[i].join();
    }
}

void Manager::runThread(BYTE* _start_ptr, ULONG _number)
{
    Entity* tmp_ptr = NULL;
    while (g_game_started) {

        for (ULONG n_entity = 0; n_entity < _number; n_entity++) {
            tmp_ptr = entity_manager->get(_start_ptr);
            for (ULONG i = 0; i < system_counter; i++) {
                a_system[i]->update(tmp_ptr);
            }
        }
    }
}

void Manager::runThreadRender()
{
    while (g_game_started) {
        {
        }
        for (ULONG i = 0; i < system_counter; i++) {
            a_system[i]->postupdate();
        }

        render_system->preupdate();
        BYTE* tmp_ptr = entity_manager->ptr();
        while (Entity* entity = entity_manager->get(tmp_ptr)) {
            render_system->update(entity);
        }
        render_system->postupdate();
    }
}

void Manager::runThreadEvent()
{
    while (g_game_started) {

        event_system->preupdate();
        BYTE* tmp_ptr = entity_manager->ptr();
        while (Entity* entity = entity_manager->get(tmp_ptr)) {
            event_system->update(entity);
        }
        event_system->postupdate();
    }
}