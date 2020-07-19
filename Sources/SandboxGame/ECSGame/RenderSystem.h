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

class RenderSystem : SESystem {
private:
    int64_t tloop1;
    int64_t tloop2;
    FLOAT count_fps = 0;

    CDrawPort* dp = NULL;
    CViewPort* vp = NULL;

public:
    void preinit() override {}
    void init(SEEntity* entity) override;
    void postinit() override {}

    void initWindow(component_window* _window);

    void preupdate() override;
    void update(SEEntity* entity) override;
    void postupdate() override;

    void updateMousePos(component_mouse* _mouse);
    void updateCursor(component_mouse* _mouse, component_texture* _texture);
    void updateTexture(component_position* _position, component_texture* _texture);
    void updateText(component_position* _position, component_text* _text);
    void updateButton(component_position* _position, component_text* _text, component_button* _button, component_mousefocus* _mousefocus);
    void updateBorder(component_position* _position);
    void updateId(SEEntity* _entity, component_position* _position);
    void updatePosition(component_camera* _camera);
    void updateFps();
    void updateWorld(component_camera* _camera);

    void destroyWindow(component_window* _window);
};

#endif