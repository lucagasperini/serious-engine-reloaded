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
    event_number = 0;
}

EventManager::~EventManager()
{
}

void EventManager::add(UINT _code, void* _parameter)
{
    std::lock_guard<std::mutex> lg(mutex_event);
    for (ULONG i = 0; i < event_number; i++) {
        if (a_event[i].code == _code) {
            a_event[i].parameter = _parameter;
            return;
        }
    }
    if (event_number < SER_ECS_EVENT_MAX) {
        a_event[event_number].code = _code;
        a_event[event_number++].parameter = _parameter;
    }
}

void EventManager::add(const Event& _event)
{
    std::lock_guard<std::mutex> lg(mutex_event);
    if (event_number < SER_ECS_EVENT_MAX)
        a_event[event_number++] = _event;
}

Event* EventManager::get()
{
    if (event_number)
        return a_event;
    return NULL;
}

void EventManager::remove(UINT _event)
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

void EventManager::remove()
{
    if (event_number > 0) {
        std::lock_guard<std::mutex> lg(mutex_event);
        for (ULONG i = 0; i < event_number; i++)
            a_event[i] = a_event[i + 1];

        event_number--;
    }
}

void EventManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex_event);
    memset(a_event, 0, SER_ECS_EVENT_MAX * sizeof(Event));
    event_number = 0;
}

void* EventManager::search(UINT _event)
{
    for (ULONG i = 0; i < event_number; i++)
        if (a_event[i].code == _event)
            return a_event[i].parameter;
    return NULL;
}

BOOL EventManager::search(UINT _event, void* _parameter)
{
    for (ULONG i = 0; i < event_number; i++)
        if (a_event[i].code == _event && a_event[i].parameter == _parameter)
            return TRUE;
    return FALSE;
}