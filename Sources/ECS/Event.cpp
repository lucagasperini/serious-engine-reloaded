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
    memset(a_event, 0, sizeof(BYTE*) * SER_ECS_EVENT_MAX);
    counter = 0;
}

EventManager::~EventManager()
{
    for (UINT i = 0; i < SER_ECS_EVENT_MAX; i++)
        if (a_event[i])
            delete[] a_event[i];
    delete[] a_event;
    //a_event = NULL;
    counter = 0;
}

void EventManager::add(UINT _code, void* _arg, ULONG _size)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = new BYTE[_size];
    memcpy(a_event[_code], _arg, _size);
    counter++;
}

void EventManager::add(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    std::lock_guard<std::mutex> lg(mutex);
    a_event[_code] = new BYTE(0);
    counter++;
}

void* EventManager::get(UINT _code)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    return a_event[_code];
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
    for (UINT i = 0; i < SER_ECS_EVENT_MAX; i++)
        if (a_event[i])
            delete[] a_event[i];

    memset(a_event, 0, SER_ECS_EVENT_MAX * sizeof(BYTE*));
    counter = 0;
}