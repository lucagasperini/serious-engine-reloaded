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

#include "Keybind.h"
#include <Engine/Base/Assert.h>

using namespace SER;

KeybindManager::KeybindManager()
{
    counter = 0;
}

KeybindManager::~KeybindManager()
{
}

void KeybindManager::add(UINT _code, ULONG _key)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    ASSERT(counter + 1 < SER_ECS_KEYBIND_MAX);
    a_keybind[counter++] = Keybind {
        _key, _code, Event { 0, 0 }
    };
}

void KeybindManager::add(UINT _code, ULONG _key, Event& _event)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    ASSERT(counter + 1 < SER_ECS_KEYBIND_MAX);
    a_keybind[counter++] = Keybind {
        _key, _code, _event
    };
}

void KeybindManager::add(UINT _code, ULONG _key, void* _arg, UINT _size)
{
    ASSERT(_code < SER_ECS_EVENT_MAX);
    ASSERT(counter + 1 < SER_ECS_KEYBIND_MAX);

    BYTE* ev_mem = new BYTE(_size);
    memcpy(ev_mem, _arg, _size);

    a_keybind[counter++] = Keybind {
        _key, _code, Event { _size, ev_mem }
    };
}

Keybind* KeybindManager::get(ULONG _key)
{
    for (UINT i = 0; i < SER_ECS_KEYBIND_MAX; i++) {
        if (a_keybind[i].key == _key && _key) {
            return &a_keybind[i];
        }
    }
}