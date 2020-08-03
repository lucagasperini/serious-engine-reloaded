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

#ifndef SER_ECS_EVENT_H
#define SER_ECS_EVENT_H

#include <Engine/Base/Types.h>
#include <mutex>

namespace SER {

class EventManager {
private:
    BYTE** a_event;
    ULONG counter;
    ULONG event_number;
    ULONG mem_size;

    std::mutex mutex;

public:
    EventManager();
    ~EventManager();

    void grow(ULONG _new);

    void add(UINT _code, void* _arg, ULONG _size);
    inline void add(UINT _code) { add(_code, new BYTE(0), sizeof(BYTE)); }

    void* get(UINT _code);

    void remove(UINT _code);
    void removeAll();

    inline ULONG count() { return counter; }
};

}
#endif