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

using namespace SER;

EventManager::EventManager()
{
    memset(a_event, 0, sizeof(Event) * SER_ECS_EVENT_MAX);
    counter = 0;
}

EventManager::~EventManager()
{
}

void EventManager::add(UINT _code, void* _parameter)
{
    std::lock_guard<std::mutex> lg(mutex);
    for (ULONG i = 0; i < counter; i++) {
        if (a_event[i].code == _code) {
            a_event[i].parameter = _parameter;
            return;
        }
    }
    if (counter < SER_ECS_EVENT_MAX) {
        a_event[counter].code = _code;
        a_event[counter++].parameter = _parameter;
    }
}

void EventManager::add(const Event& _event)
{
    std::lock_guard<std::mutex> lg(mutex);
    if (counter < SER_ECS_EVENT_MAX)
        a_event[counter++] = _event;
}

Event* EventManager::get()
{
    if (counter)
        return a_event;
    return NULL;
}

void EventManager::remove(UINT _event)
{
    if (counter <= 0)
        return;
    for (ULONG i = 0; i < counter; i++) {
        if (a_event[i].code == _event) {
            std::lock_guard<std::mutex> lg(mutex);
            a_event[i].code = a_event[i + 1].code;
            a_event[i].parameter = a_event[i + 1].parameter;
            counter--;
        }
    }
}

void EventManager::remove()
{
    if (counter > 0) {
        std::lock_guard<std::mutex> lg(mutex);
        for (ULONG i = 0; i < counter; i++)
            a_event[i] = a_event[i + 1];

        counter--;
    }
}

void EventManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    memset(a_event, 0, SER_ECS_EVENT_MAX * sizeof(Event));
    counter = 0;
}

void* EventManager::search(UINT _event)
{
    for (ULONG i = 0; i < counter; i++)
        if (a_event[i].code == _event)
            return a_event[i].parameter;
    return NULL;
}

BOOL EventManager::search(UINT _event, void* _parameter)
{
    for (ULONG i = 0; i < counter; i++)
        if (a_event[i].code == _event && a_event[i].parameter == _parameter)
            return TRUE;
    return FALSE;
}