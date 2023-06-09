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

#ifndef SER_SBGAME_EVENTSYSTEM_H
#define SER_SBGAME_EVENTSYSTEM_H

#include "Entity.h"
#include <SER/ECS/System.h>

#define SER_KEYBIND_MAX 256

struct EventKeybind {
    UINT code;
    ULONG key;
    void* arg;
    ULONG size;
};

class EventSystem : public SER::System {
private:
    int x = 0;
    int y = 0;
    int delta_x = 0;
    int delta_y = 0;
    int old_x = 0;
    int old_y = 0;

    SDL_Event event;

    EventKeybind a_keybind[SER_KEYBIND_MAX];
    ULONG keybind_counter = 0;

public:
    void addKeybind(const EventKeybind& _keybind);
    void preinit() override {}
    void init(SER::Entity* _entity) override { System::init(_entity); }
    void postinit() override {}

    void preupdate() override;
    void update(SER::Entity* _entity) override;
    void postupdate() override {};
};

#endif