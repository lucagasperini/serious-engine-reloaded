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
#define SER_ECS_EVENT_MAX 256

namespace SER {

struct Event {
    UINT code;
    void* parameter;
};

class EventManager {
private:
    Event a_event[SER_ECS_EVENT_MAX];
    ULONG event_number;

    std::mutex mutex_event;

public:
    EventManager();
    ~EventManager();

    void add(UINT _code, void* _parameter);
    void add(const Event& _event);

    Event* get();

    void* search(UINT _event);
    BOOL search(UINT _event, void* _parameter);

    void remove(UINT _event);
    void remove();
    void removeAll();

    inline ULONG count() { return event_number; }
};

}
#endif