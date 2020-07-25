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
    a_event = NULL;
    counter = 0;
    event_number = 0;
    mem_size = 0;
}

EventManager::~EventManager()
{
    for (UINT i = 0; i < event_number; i++)
        if (a_event[i])
            delete[] a_event[i];
    delete[] a_event;
    a_event = NULL;

    mem_size = 0;
    event_number = 0;
    counter = 0;
}

//TODO: Add a runtime grow with memory copy with realloc
void EventManager::grow(ULONG _add)
{
    event_number += _add;
    mem_size += event_number * sizeof(BYTE*);
    a_event = (BYTE**)calloc(event_number, sizeof(BYTE*));
    counter = 0;
}

void EventManager::add(UINT _code, void* _arg, ULONG _size)
{
    ASSERT(_code < event_number);
    std::lock_guard<std::mutex> lg(mutex);

    BYTE* tmp_ptr = NULL;
    if (!a_event[_code]) {
        tmp_ptr = (BYTE*)malloc(sizeof(UINT) + _size);
    } else {
        tmp_ptr = a_event[_code];
    }
    memcpy(tmp_ptr, &_size, sizeof(UINT));
    memcpy(tmp_ptr + sizeof(UINT), _arg, _size);

    mem_size += _size;
    a_event[_code] = tmp_ptr;
    counter++;
}

void* EventManager::get(UINT _code)
{
    ASSERT(_code < event_number);
    if (a_event[_code])
        return a_event[_code] + sizeof(UINT);
    return NULL;
}

void EventManager::remove(UINT _code)
{
    ASSERT(_code < event_number);
    ASSERT(a_event[_code]);
    std::lock_guard<std::mutex> lg(mutex);
    UINT* tmp_ptr_size = (UINT*)a_event[_code];
    mem_size -= *tmp_ptr_size;
    free(a_event[_code]);
    a_event[_code] = NULL;
    counter--;
}

void EventManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    BYTE* tmp_ptr = NULL;
    for (UINT i = 0; i < event_number; i++) {
        tmp_ptr = a_event[i];
        if (tmp_ptr)
            free(tmp_ptr);
    }
    mem_size = event_number * sizeof(BYTE*);
    memset(a_event, 0, mem_size);
    counter = 0;
}