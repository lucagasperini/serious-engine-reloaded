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

#include "RenderSystem.h"

extern CDrawPort* main_dp;
extern CFontData* main_font_small;
extern CFontData* main_font_medium;
extern COLOR fallback_color;
extern BOOL dbg_draw_border;
extern BOOL dbg_draw_id;
extern BOOL dbg_draw_position;
extern BOOL dbg_draw_fps;
extern ULONG dbg_count_fps;
extern CWorld* world_data;

void RenderSystem::render_position(component_camera* _camera)
{
    PIXaabbox2D box(PIX2D(0, 0), PIX2D(200, 100));
    main_dp->SetFont(main_font_small);
    char* buffer = new char[128];
    float px = _camera->cam_pos(1);
    float py = _camera->cam_pos(2);
    float pz = _camera->cam_pos(3);
    float ax = _camera->cam_rot(1);
    float ay = _camera->cam_rot(2);
    float az = _camera->cam_rot(3);

    snprintf(buffer, 128, "X: %f\nY: %f\nZ: %f\nAX: %f \nAY: %f \nAZ: %f", px, py, pz, ax, ay, az);
    main_dp->PutText(buffer, box.Min()(1), box.Min()(2), fallback_color);
}

void RenderSystem::render_fps()
{
    PIXaabbox2D box(PIX2D(0, 100), PIX2D(100, 100));
    main_dp->SetFont(main_font_medium);
    char* buffer = new char[16];
    snprintf(buffer, 16, "FPS: %u", dbg_count_fps);
    main_dp->PutText(buffer, box.Min()(1), box.Min()(2), fallback_color);
}

void RenderSystem::init(SEEntity* entity)
{
    component_texture* texture = dynamic_cast<component_texture*>((SEEntity*)entity);
    if (texture)
        init_texture(texture);
}

void RenderSystem::init_texture(component_texture* _texture)
{
    _texture->tex_data.SetData_t(_texture->tex_file);
}

void RenderSystem::update(SEEntity* entity)
{
    component_camera* camera = dynamic_cast<component_camera*>((SEEntity*)entity);
    if (camera)
        render_world(camera);

    component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
    if (dbg_draw_border && position)
        render_border(position);
    if (dbg_draw_id && position)
        render_id(entity, position);
    if (dbg_draw_position && camera)
        render_position(camera);
    if (dbg_draw_fps)
        render_fps();

    component_texture* texture = dynamic_cast<component_texture*>((SEEntity*)entity);
    if (position && texture)
        render_texture(position, texture);

    component_text* text = dynamic_cast<component_text*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    component_button* button = dynamic_cast<component_button*>((SEEntity*)entity);
    if (position && text && button && mousefocus)
        render_button(position, text, button, mousefocus);
    else if (position && text)
        render_text(position, text);
}

void RenderSystem::render_texture(component_position* _position, component_texture* _texture)
{
    main_dp->PutTexture(&_texture->tex_data, PIXaabbox2D(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h)));
}

void RenderSystem::render_text(component_position* _position, component_text* _text)
{
    main_dp->SetFont(&_text->txt_fontdata);
    main_dp->SetTextScaling(1.0f);
    main_dp->SetTextAspect(1.0f);
    main_dp->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    if (_text->txt_align == -1)
        main_dp->PutText(_text->txt_str, box.Min()(1), box.Min()(2), _text->txt_color);
    else if (_text->txt_align == +1)
        main_dp->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), _text->txt_color);
    else
        main_dp->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), _text->txt_color);
}

void RenderSystem::render_button(component_position* _position, component_text* _text, component_button* _button, component_mousefocus* _mousefocus)
{
    main_dp->SetFont(&_text->txt_fontdata);
    main_dp->SetTextScaling(1.0f);
    main_dp->SetTextAspect(1.0f);
    main_dp->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    COLOR col;
    if (_mousefocus->mf_focus)
        col = _button->btn_color2;
    else
        col = _text->txt_color;

    if (_text->txt_align == -1)
        main_dp->PutText(_text->txt_str, box.Min()(1), box.Min()(2), col);
    else if (_text->txt_align == +1)
        main_dp->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), col);
    else
        main_dp->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), col);
}

void RenderSystem::render_border(component_position* _position)
{
    main_dp->DrawBorder(_position->pos_x, _position->pos_y, _position->pos_w, _position->pos_h, fallback_color);
}

void RenderSystem::render_id(SEEntity* _entity, component_position* _position)
{
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));
    main_dp->SetFont(main_font_small);
    char* buffer = new char[32];
    snprintf(buffer, 32, "%i", _entity->id);
    main_dp->PutText(buffer, box.Min()(1), box.Min()(2), fallback_color);
}

void RenderSystem::render_world(component_camera* _camera)
{
    CPlacement3D plCamera;
    plCamera.pl_PositionVector = _camera->cam_pos;
    plCamera.pl_OrientationAngle = _camera->cam_rot;

    // init projection parameters
    CPerspectiveProjection3D prPerspectiveProjection;
    prPerspectiveProjection.FOVL() = _camera->cam_fov;
    prPerspectiveProjection.ScreenBBoxL() = FLOATaabbox2D(
        FLOAT2D(0.0f, 0.0f), FLOAT2D((float)main_dp->GetWidth(), (float)main_dp->GetHeight()));
    prPerspectiveProjection.AspectRatioL() = 1.0f;
    prPerspectiveProjection.FrontClipDistanceL() = 0.3f;

    CAnyProjection3D prProjection;
    prProjection = prPerspectiveProjection;

    // set up viewer position
    prProjection->ViewerPlacementL() = plCamera;
    // render the view
    RenderView(*world_data, *(CEntity*)NULL, prProjection, *main_dp);
}