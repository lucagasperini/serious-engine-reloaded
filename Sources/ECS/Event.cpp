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

#include "Event.h"
#include <Engine/Base/Assert.h>

using namespace SER;

EventManager::EventManager()
{
    a_event = new Event[SER_ECS_EVENT_MAX];
    memset(a_event, 0, sizeof(Event) * SER_ECS_EVENT_MAX);
    counter = 0;
}

EventManager::~EventManager()
{
}

void EventManager::add(UINT _code, void* _arg, ULONG _size)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    BYTE* ev_mem = new BYTE(_size);
    memcpy(ev_mem, _arg, _size);
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = Event { _size, ev_mem };
    counter++;
}

void EventManager::add(UINT _code, const Event& _event)
{
    add(_code, _event.arg, _event.size);
}

void EventManager::add(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = Event { 0, NULL };
    counter++;
}

Event EventManager::get(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    return a_event[_code];
}

void* EventManager::getArg(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    return a_event[_code].arg;
}

void EventManager::remove(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    delete a_event[_code].arg;
    a_event[_code].arg = NULL;
    a_event[_code].size = 0;
    counter--;
}

void EventManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    memset(a_event, 0, SER_ECS_EVENT_MAX * sizeof(Event));
    counter = 0;
}