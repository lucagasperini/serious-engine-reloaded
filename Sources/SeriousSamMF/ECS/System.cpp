#include "System.h"
#include "Manager.h"

extern POINT cursor;
extern SDL_Event* event;
extern CDrawPort* main_dp;
extern ECSManager* manager;

void PositionSystem::init()
{
    ULONG width = main_dp->GetWidth();
    ULONG height = main_dp->GetHeight();
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!position)
            continue;

        init_scale(position, width, height);

        SEAlignComponent* align = dynamic_cast<SEAlignComponent *>((SEEntity*)entity);
        if (align)
            init_align(position, align);
    }
    old_width = width;
    old_height = height;
}

void PositionSystem::init_scale(SEPositionComponent* _position, ULONG _width, ULONG _height)
{
    FLOAT scaleX = (FLOAT)_width / (FLOAT)old_width;
    FLOAT scaleY = (FLOAT)_height / (FLOAT)old_height;

    _position->x = (FLOAT)_position->x * scaleX;
    _position->y = (FLOAT)_position->y * scaleY;
    _position->w = (FLOAT)_position->w * scaleX;
    _position->h = (FLOAT)_position->h * scaleY;
}

void PositionSystem::init_align(SEPositionComponent* _position, SEAlignComponent* _align)
{
    ULONG center_x = main_dp->GetWidth() /2;
    ULONG center_y = main_dp->GetHeight() /2;

    switch (_align->align_x)
    {
    case -2:
        _position->x = 0;
        break;
    case -1:
        _position->x = center_x - _position->w;
        break;
    case 0:
        _position->x = center_x - _position->w / 2;
        break;
    case 1:
        _position->x = center_x;
        break;
    case 2:
        _position->x = center_x * 2 - _position->w;
        break;
    }
    switch (_align->align_y)
    {
    case -2:
        _position->y = 0;
        break;
    case -1:
        _position->y = center_y - _position->h;
        break;
    case 0:
        _position->y = center_y - _position->h / 2;
        break;
    case 1:
        _position->y = center_y;
        break;
    case 2:
        _position->y = center_y * 2 - _position->y;
        break;
    }
}

void PositionSystem::update()
{
    //DO SOMETHING
}

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

void InputSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!position)
            continue;

        SEButtonComponent* button = dynamic_cast<SEButtonComponent *>((SEEntity*)entity);
        if (button) 
            input_button(position, button);
    }
}

void InputSystem::input_button(SEPositionComponent* _position, SEButtonComponent* _button)
{
    if (_position->x < cursor.x &&
        _position->y < cursor.y &&
        _position->x + _position->w > cursor.x &&
        _position->y + _position->h > cursor.y)
    {
        _button->focus = TRUE;
    }
    else
    {
        _button->focus = FALSE;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN &&
        event->button.button == SDL_BUTTON_LEFT &&
        _button->function != NULL &&
        _button->focus)
    {
        _button->function();
    }
}