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

#ifndef SER_ECS_RENDERSYSTEM_H
#define SER_ECS_RENDERSYSTEM_H

#include "Entity.h"
#include "System.h"

class RenderSystem : SESystem {
private:
    int64_t tloop1;
    int64_t tloop2;
    FLOAT count_fps = 0;

public:
    void preinit() override {}
    void init(SEEntity* entity) override;
    void postinit() override {}

    void init_texture(component_texture* _texture);

    void preupdate() override;
    void update(SEEntity* entity) override;
    void postupdate() override;

    void render_texture(component_position* _position, component_texture* _texture);
    void render_text(component_position* _position, component_text* _text);
    void render_button(component_position* _position, component_text* _text, component_button* _button, component_mousefocus* _mousefocus);
    void render_border(component_position* _position);
    void render_id(SEEntity* _entity, component_position* _position);
    void render_position(component_camera* _camera);
    void render_fps();
    void render_world(component_camera* _camera);
};

#endif