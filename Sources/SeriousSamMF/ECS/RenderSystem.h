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

#ifndef SSMF_RENDERSYSTEM_H
#define SSMF_RENDERSYSTEM_H

#include "Entity.h"

class RenderSystem {
public:
    void init(SEEntity* entity);
    void init_texture(SETextureComponent* _texture);

    void update(SEEntity* entity);
    void render_texture(SEPositionComponent* _position, SETextureComponent* _texture);
    void render_text(SEPositionComponent* _position, SETextComponent* _text);
    void render_button(SEPositionComponent* _position, SETextComponent* _text, SEButtonComponent* _button, SEMouseFocusComponent* _mousefocus);
    void render_border(SEPositionComponent* _position);
    void render_id(SEEntity* _entity, SEPositionComponent* _position);
    void render_position(SECameraComponent* _camera);
    void render_fps();
    void render_world(SECameraComponent* _camera);
};

#endif