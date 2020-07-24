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
    Event* ev = new Event { _size, ev_mem };
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = ev;
    counter++;
}

void EventManager::add(UINT _code, const Event& _event)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    memcpy(a_event[_code], &_event, sizeof(Event));
    counter++;
}

void EventManager::add(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    Event* tmp_ev = new Event { 0, NULL };
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = tmp_ev;
    counter++;
}

Event* EventManager::get(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    return a_event[_code];
}

void* EventManager::getArg(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    return a_event[_code]->arg;
}

void EventManager::remove(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    delete a_event[_code];
    a_event[_code] = NULL;
    counter--;
}

void EventManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    memset(a_event, 0, SER_ECS_EVENT_MAX * sizeof(Event));
    counter = 0;
}