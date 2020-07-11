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
    void render_world(SECameraComponent* _camera);
};

#endif