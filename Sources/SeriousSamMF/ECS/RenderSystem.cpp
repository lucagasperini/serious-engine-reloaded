#include "RenderSystem.h"
#include "Manager.h"

extern ECSManager* manager;
extern CDrawPort* main_dp;
extern CFontData* main_fb_font;
extern COLOR main_fb_color;

extern CPlayerEntity *pen;

void RenderSystem::render_position()
{
    PIXaabbox2D box(PIX2D(0, 0), PIX2D(200, 100));
    main_dp->SetFont(main_fb_font);
    char* buffer = new char[128];
    float px = pen->en_plPlacement.pl_PositionVector(1);
    float py = pen->en_plPlacement.pl_PositionVector(2); 
    float pz = pen->en_plPlacement.pl_PositionVector(3);
    float ax = pen->en_plPlacement.pl_OrientationAngle(1);
    float ay = pen->en_plPlacement.pl_OrientationAngle(2);
    float az = pen->en_plPlacement.pl_OrientationAngle(3);

    snprintf(buffer, 128, "X: %f\nY: %f\nZ: %f\nAX: %f \nAY: %f \nAZ: %f", px, py, pz, ax, ay, az);
    main_dp->PutText(buffer, box.Min()(1), box.Min()(2), main_fb_color);
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
        if(dbg_draw_id)
            render_id(entity, position);
        if(dbg_draw_position)
            render_position();

        SETextureComponent* texture = dynamic_cast<SETextureComponent *>((SEEntity*)entity);
        if (texture) 
            render_texture(position, texture);

        SETextComponent* text = dynamic_cast<SETextComponent *>((SEEntity*)entity);
        SEMouseFocusComponent* mousefocus = dynamic_cast<SEMouseFocusComponent *>((SEEntity*)entity);
        SEButtonComponent* button = dynamic_cast<SEButtonComponent *>((SEEntity*)entity);
        if (text && button) 
            render_button(position, text, button, mousefocus);
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

void RenderSystem::render_button(SEPositionComponent* _position, SETextComponent* _text, SEButtonComponent* _button, SEMouseFocusComponent* _mousefocus)
{
    main_dp->SetFont(&_text->fontdata);
    main_dp->SetTextScaling(1.0f);
    main_dp->SetTextAspect(1.0f);
    main_dp->SetTextMode(_text->textmode);
    PIXaabbox2D box(PIX2D(_position->x, _position->y), PIX2D(_position->x + _position->w, _position->y + _position->h));

    COLOR col;
    if(_mousefocus->semf_focus)
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
     main_dp->DrawBorder(_position->x, _position->y, _position->w, _position->h, main_fb_color);
}

void RenderSystem::render_id(SEEntity* _entity, SEPositionComponent* _position)
{
    PIXaabbox2D box(PIX2D(_position->x, _position->y), PIX2D(_position->x + _position->w, _position->y + _position->h));
    main_dp->SetFont(main_fb_font);
    char* buffer = new char[32];
    snprintf(buffer, 32, "%i", _entity->id);
    main_dp->PutText(buffer, box.Min()(1), box.Min()(2), main_fb_color);
}