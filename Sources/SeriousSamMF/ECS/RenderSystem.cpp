#include "RenderSystem.h"
#include "Manager.h"

extern ECSManager* manager;
extern CDrawPort* main_dp;

void RenderSystem::init()
{
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SETextureComponent* texture = dynamic_cast<SETextureComponent *>((SEEntity*)entity);
        if (texture) 
            init_texture(texture);
    }
}

void RenderSystem::init_texture(SETextureComponent *_texture)
{
    _texture->tex.SetData_t(_texture->fntex);
}

void RenderSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!position)
            continue;
        if(dbg_draw_border)
            render_border(position);

        SETextureComponent* texture = dynamic_cast<SETextureComponent *>((SEEntity*)entity);
        if (texture) 
            render_texture(position, texture);

        SETextComponent* text = dynamic_cast<SETextComponent *>((SEEntity*)entity);
        SEButtonComponent* button = dynamic_cast<SEButtonComponent *>((SEEntity*)entity);
        if (text && button) 
            render_button(position, text, button);
        else if (text) 
            render_text(position, text);
        else
            continue;
    }
}

void RenderSystem::render_texture(SEPositionComponent* _position, SETextureComponent* _texture)
{
    main_dp->PutTexture(&_texture->tex, PIXaabbox2D(
                                           PIX2D(_position->x, _position->y),
                                           PIX2D(_position->x + _position->w, _position->y + _position->h)));
}

void RenderSystem::render_text(SEPositionComponent* _position, SETextComponent* _text)
{
    main_dp->SetFont(&_text->fontdata);
    main_dp->SetTextScaling(1.0f);
    main_dp->SetTextAspect(1.0f);
    main_dp->SetTextMode(_text->textmode);
    PIXaabbox2D box(PIX2D(_position->x, _position->y), PIX2D(_position->x + _position->w, _position->y + _position->h));

    if (_text->align == -1)
        main_dp->PutText(_text->str, box.Min()(1), box.Min()(2), _text->color);
    else if (_text->align == +1)
        main_dp->PutTextR(_text->str, box.Max()(1), box.Min()(2), _text->color);
    else
        main_dp->PutTextC(_text->str, box.Center()(1), box.Min()(2), _text->color);
}

void RenderSystem::render_button(SEPositionComponent* _position, SETextComponent* _text, SEButtonComponent* _button)
{
    main_dp->SetFont(&_text->fontdata);
    main_dp->SetTextScaling(1.0f);
    main_dp->SetTextAspect(1.0f);
    main_dp->SetTextMode(_text->textmode);
    PIXaabbox2D box(PIX2D(_position->x, _position->y), PIX2D(_position->x + _position->w, _position->y + _position->h));

    COLOR col;
    if(_button->focus)
        col = _button->color2;
    else
        col = _text->color;

    if (_text->align == -1)
        main_dp->PutText(_text->str, box.Min()(1), box.Min()(2), col);
    else if (_text->align == +1)
        main_dp->PutTextR(_text->str, box.Max()(1), box.Min()(2), col);
    else
        main_dp->PutTextC(_text->str, box.Center()(1), box.Min()(2), col);
}

void RenderSystem::render_border(SEPositionComponent* _position)
{
     main_dp->DrawBorder(_position->x, _position->y, _position->w, _position->h, C_BLACK|0xff);
}