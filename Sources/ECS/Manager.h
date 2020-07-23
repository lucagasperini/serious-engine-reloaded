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

#include "Entity.h"
#include "Event.h"
#include "Keybind.h"
#include "System.h"
#include <Engine/Engine.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#define SER_ECS_SYSTEM_MAX 256
#define SER_ECS_EVENT_MAX 256

#define SER_GET_COMPONENT(_name, _component, _entity) _component* _name = dynamic_cast<_component*>(_entity)

#define SER_ADD_ENTITY(_name, _entity) SER::ECS::Manager::getEntityManager()->add((SEEntity*)_name, sizeof(_entity));

namespace SER::ECS {

class Manager {
private:
    static ULONG system_counter;

    static System* a_system[SER_ECS_SYSTEM_MAX];

    static std::thread* a_thread;
    static std::thread thread_event;

    static BYTE** a_thread_memory;

    static ULONG thread_number;

    static SEEvent a_event[SER_ECS_EVENT_MAX];
    static ULONG event_number;

    static std::mutex mutex_event;

    static std::mutex mutex_update;
    static std::mutex mutex_counter;
    static std::mutex mutex_end_frame;
    static std::condition_variable cv_update;
    static ULONG number_update;
    static BOOL wait_update;

    static System* render_system;
    static System* event_system;

    static std::mutex mutex_render;
    static std::condition_variable cv_render;
    static BOOL wait_render;

    static ULONG loop_status;
    static BOOL is_end_frame;

    static EntityManager* entity_manager;

public:
    static SEEvent* getEvent();

    static void* searchEvent(UINT _event);
    static BOOL searchEvent(UINT _event, void* _parameter);

    static void removeEvent(UINT _event);
    static void removeEvent();
    static void removeAllEvent();

    Manager();
    ~Manager();

    static inline EntityManager* getEntityManager() { return entity_manager; }
    static inline void setRenderSystem(System* _render_system) { render_system = _render_system; };
    static inline void setEventSystem(System* _event_system) { event_system = _event_system; };

    static void init(ULONG _entity_space, ULONG _event_space);

    static void setThreadNumber(ULONG _thread_number);
    static inline ULONG getThreadNumber() { return thread_number; }

    static void splitThreadMemory();

    static void run();
    static void quit();

    static void runThread(BYTE* _start_ptr, ULONG _number);
    static void runThreadRender();
    static void runThreadEvent();

    static void addSystem(System* _system);
    static void addEvent(UINT _code, void* _parameter);
    static void addEvent(const SEEvent& _event);
};
}
#endif