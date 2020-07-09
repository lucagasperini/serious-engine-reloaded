#ifndef SSMF_SYSTEM_H
#define SSMF_SYSTEM_H

#include <Engine/Base/Types.h>
#include <Engine/Engine.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Font.h>
#include "Entity.h"

class SESystem
{
    public:
    virtual void init(){};
    virtual void update(){};
};


class PositionSystem : SESystem
{
    public:
    ULONG old_width = 1920;
    ULONG old_height = 1080;

    void init() override;
    void init_scale(SEPositionComponent* _position, ULONG width, ULONG height);
    void init_align(SEPositionComponent* _position, SEAlignComponent* _align);

    void update();
};

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

class InputSystem : SESystem
{
    public:

    void update() override;
    void input_button(SEPositionComponent* _position, SEButtonComponent* _button);
};

#endif