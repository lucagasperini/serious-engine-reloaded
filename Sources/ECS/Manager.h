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
#include "System.h"
#include <Engine/Engine.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#define SER_ECS_SYSTEM_MAX 256

#define SER_GET_COMPONENT(_name, _component, _entity) _component* _name = dynamic_cast<_component*>(_entity)

#define SER_ADD_ENTITY(_name, _entity) SER::Manager::getEntityManager()->add((Entity*)_name, sizeof(_entity))

#define SER_ADD_EVENT_NOARG(_event) Manager::getEventManager()->add(_event)

#define SER_ADD_EVENT(_event, _arg, _type) Manager::getEventManager()->add(_event, _arg, sizeof(_type))

#define SER_ADD_EVENT_ARRAY(_event, _arg, _type, _number) Manager::getEventManager()->add(_event, _arg, sizeof(_type) * _number)

#define SER_GET_EVENT_ARG(_name, _type, _event) _type* _name = (_type*)Manager::getEventManager()->get(_event)

#define SER_GET_EVENT(_event) Manager::getEventManager()->get(_event)

#define SER_REMOVE_EVENT(_event) Manager::getEventManager()->remove(_event)

namespace SER {

class Manager {
private:
    static ULONG system_counter;

    static System* a_system[SER_ECS_SYSTEM_MAX];

    static std::thread* a_thread;
    static std::thread thread_event;

    static BYTE** a_thread_memory;

    static ULONG thread_number;

    static System* render_system;
    static System* event_system;

    static ULONG loop_status;
    static BOOL is_end_frame;

    static EntityManager* entity_manager;
    static EventManager* event_manager;

public:
    Manager();
    ~Manager();

    static inline EntityManager* getEntityManager() { return entity_manager; }
    static inline EventManager* getEventManager() { return event_manager; }

    static inline void setRenderSystem(System* _render_system) { render_system = _render_system; };
    static inline void setEventSystem(System* _event_system) { event_system = _event_system; };

    static void init(ULONG _entity_space, ULONG _event_limit);

    static void setThreadNumber(ULONG _thread_number);
    static inline ULONG getThreadNumber() { return thread_number; }

    static void splitThreadMemory();

    static void run();
    static void quit();

    static void runThread(BYTE* _start_ptr, ULONG _number);
    static void runThreadRender();
    static void runThreadEvent();

    static void addSystem(System* _system);
};
}
#endif