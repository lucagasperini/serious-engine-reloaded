#ifndef SSMF_RENDERSYSTEM_H
#define SSMF_RENDERSYSTEM_H

#include "System.h"

class RenderSystem : SESystem
{
    public:
    BOOL dbg_draw_border = FALSE;
    void init() override;
    void init_texture(SETextureComponent* _texture);

    void update() override;
    void render_texture(SEPositionComponent* _position, SETextureComponent* _texture);
    void render_text(SEPositionComponent* _position, SETextComponent* _text);
    void render_button(SEPositionComponent* _position, SETextComponent* _text, SEButtonComponent* _button);
    void render_border(SEPositionComponent* _position);
};

#endif