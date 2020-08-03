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

#include "Setting.h"

#include <Engine/Base/Assert.h>

using namespace SER;

SettingManager::SettingManager()
{
    a_setting = NULL;
    counter = 0;
    mem_size = 0;
}

SettingManager::~SettingManager()
{
    for (UINT i = 0; i < counter; i++)
        if (a_setting[i])
            delete[] a_setting[i];
    delete[] a_setting;
    a_setting = NULL;

    mem_size = 0;
    counter = 0;
}

//TODO: Add a runtime grow with memory copy with realloc
void SettingManager::grow(ULONG _add)
{
    counter += _add;
    mem_size += counter * sizeof(BYTE*);
    a_setting = (BYTE**)calloc(counter, sizeof(BYTE*));
}

void SettingManager::add(UINT _code, void* _arg, ULONG _size)
{
    ASSERT(_code < counter);
    std::lock_guard<std::mutex> lg(mutex);

    BYTE* tmp_ptr = NULL;
    if (!a_setting[_code]) {
        tmp_ptr = (BYTE*)malloc(sizeof(UINT) + _size);
    } else {
        tmp_ptr = a_setting[_code];
    }
    memcpy(tmp_ptr, &_size, sizeof(UINT));
    memcpy(tmp_ptr + sizeof(UINT), _arg, _size);

    mem_size += _size;
    a_setting[_code] = tmp_ptr;
}

void* SettingManager::get(UINT _code)
{
    ASSERT(_code < counter);
    if (a_setting[_code])
        return a_setting[_code] + sizeof(UINT);
    return NULL;
}

void SettingManager::remove(UINT _code)
{
    ASSERT(_code < counter);
    if (a_setting[_code]) {
        std::lock_guard<std::mutex> lg(mutex);
        UINT* tmp_ptr_size = (UINT*)a_setting[_code];
        mem_size -= *tmp_ptr_size;
        free(a_setting[_code]);
        a_setting[_code] = NULL;
    }
}

void SettingManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    BYTE* tmp_ptr = NULL;
    for (UINT i = 0; i < counter; i++) {
        tmp_ptr = a_setting[i];
        if (tmp_ptr)
            free(tmp_ptr);
    }
    mem_size = counter * sizeof(BYTE*);
    memset(a_setting, 0, mem_size);
}