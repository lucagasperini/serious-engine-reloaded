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

#ifndef SER_SBGAME_RENDERSYSTEM_H
#define SER_SBGAME_RENDERSYSTEM_H

#include "Entity.h"
#include <ECS/System.h>

class RenderSystem : public SER::System {
private:
    int64_t tloop1;
    int64_t tloop2;
    FLOAT count_fps = 0;

    CDrawPort* dp = NULL;
    CViewPort* vp = NULL;

public:
    void preinit() override {}
    void init(SER::Entity* _entity) override;
    void postinit() override {}

    void initWindow(ComponentWindow* _window);

    void preupdate();
    void update(SER::Entity* _entity) override;
    void postupdate();

    void updateCursor(ComponentCursor* _cursor);
    void updateTexture(ComponentPosition* _position, ComponentTexture* _texture);
    void updateButton(ComponentPosition* _position, ComponentButton* _button);
    void updateBorder(ComponentPosition* _position);
    void updateId(SER::Entity* _entity, ComponentPosition* _position);
    void updatePosition(ComponentCamera* _camera);
    void updateFps();
    void updateWorld(ComponentCamera* _camera);

    void destroyWindow(ComponentWindow* _window);

    void eventWindow(ComponentWindow* _window);
};

#endif