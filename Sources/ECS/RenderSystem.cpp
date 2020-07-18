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
#include "Manager.h"

extern CDrawPort* g_drawport;
extern CViewPort* g_viewport;
//TODO: Create a structure to manage fonts.
//extern CFontData* main_font_small;
//extern CFontData* main_font_medium;
extern COLOR g_fb_color;
extern BOOL g_dbg_draw_border;
extern BOOL g_dbg_draw_id;
extern BOOL g_dbg_draw_position;
extern BOOL g_dbg_draw_fps;
extern BOOL g_dbg_draw_cursor;
extern CWorld* g_world_data;

void RenderSystem::preupdate()
{
    //TODO: What's the sense of calculate Milliseconds with a signed int???
    tloop1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    if (!g_drawport->Lock()) {
        //FIXME: Add some stop render flag.
        return;
    }
    // clear z-buffer
    g_drawport->FillZBuffer(ZBUF_BACK);
    g_drawport->Fill(C_BLACK | 0xff);
}

void RenderSystem::postupdate()
{
    g_drawport->Unlock();
    g_viewport->SwapBuffers();
    tloop2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    //TODO: Maybe do this 1 time at second, or render the value 1 time at second.
    if (tloop2 - tloop1)
        count_fps = 1000 / ((FLOAT)(tloop2 - tloop1));
    else
        count_fps = 0.0f;
}

void RenderSystem::render_position(component_camera* _camera)
{
    PIXaabbox2D box(PIX2D(0, 0), PIX2D(200, 100));
    //g_drawport->SetFont(main_font_small);
    char* buffer = new char[128];
    float px = _camera->cam_pos(1);
    float py = _camera->cam_pos(2);
    float pz = _camera->cam_pos(3);
    float ax = _camera->cam_rot(1);
    float ay = _camera->cam_rot(2);
    float az = _camera->cam_rot(3);

    snprintf(buffer, 128, "X: %f\nY: %f\nZ: %f\nAX: %f \nAY: %f \nAZ: %f", px, py, pz, ax, ay, az);
    g_drawport->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::render_fps()
{
    PIXaabbox2D box(PIX2D(0, 100), PIX2D(100, 100));
    //g_drawport->SetFont(main_font_medium);
    char* buffer = new char[16];
    snprintf(buffer, 16, "FPS: %f", count_fps);
    g_drawport->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::init(SEEntity* entity)
{
    component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
    component_window* window = dynamic_cast<component_window*>((SEEntity*)entity);
    if (window && position)
        init_window(window, position);

    component_texture* texture = dynamic_cast<component_texture*>((SEEntity*)entity);
    if (texture)
        init_texture(texture);
}

void RenderSystem::init_window(component_window* _window, component_position* _position)
{
    if (_window->win_pointer != NULL)
        destroy_window(_window);

    // try to set new display mode
    _pGfx->SetDisplayMode(_window->win_api, _window->win_adapter,
        _position->pos_w, _position->pos_h, _window->win_depth);

    ULONG tmp_flags = _window->win_flags;

    if (_window->win_api == GAT_OGL && !(tmp_flags & SDL_WINDOW_OPENGL))
        tmp_flags = tmp_flags | SDL_WINDOW_OPENGL;

    _window->win_pointer = SDL_CreateWindow(_window->win_title,
        _position->pos_x, _position->pos_y,
        _position->pos_w, _position->pos_h, tmp_flags);

    if (_window->win_pointer == NULL)
        FatalError(TRANS("Cannot open main window!"));

    SE_UpdateWindowHandle(_window->win_pointer);

    _pGfx->CreateWindowCanvas(_window->win_pointer, &g_viewport, &g_drawport);
#ifndef DEBUG
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    // initial screen fill and swap, just to get context running
    if (g_drawport != NULL && g_drawport->Lock()) {
        g_drawport->Fill(C_BLACK | 255);
        g_drawport->Unlock();
        g_viewport->SwapBuffers();
    }
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
    if (g_dbg_draw_border && position)
        render_border(position);
    if (g_dbg_draw_id && position)
        render_id(entity, position);
    if (g_dbg_draw_position && camera)
        render_position(camera);
    if (g_dbg_draw_fps)
        render_fps();

    component_texture* texture = dynamic_cast<component_texture*>((SEEntity*)entity);
    if (position && texture)
        render_texture(position, texture);
    component_mouse* mouse = dynamic_cast<component_mouse*>((SEEntity*)entity);
    if (mouse && g_dbg_draw_cursor)
        render_mouse_pos(mouse);
    if (mouse && texture)
        render_cursor(mouse, texture);

    component_text* text = dynamic_cast<component_text*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    component_button* button = dynamic_cast<component_button*>((SEEntity*)entity);
    if (position && text && button && mousefocus)
        render_button(position, text, button, mousefocus);
    else if (position && text)
        render_text(position, text);
}

void RenderSystem::render_mouse_pos(component_mouse* _mouse)
{
    g_drawport->DrawBorder(_mouse->mouse_cursor.x, _mouse->mouse_cursor.y,
        32, 32, g_fb_color);
}

void RenderSystem::render_cursor(component_mouse* _mouse, component_texture* _texture)
{
    g_drawport->PutTexture(&_texture->tex_data,
        PIXaabbox2D(PIX2D(_mouse->mouse_cursor.x, _mouse->mouse_cursor.y),
            PIX2D(_mouse->mouse_cursor.x + 32, _mouse->mouse_cursor.y + 32)));
}

void RenderSystem::render_texture(component_position* _position, component_texture* _texture)
{
    g_drawport->PutTexture(&_texture->tex_data, PIXaabbox2D(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h)));
}

void RenderSystem::render_text(component_position* _position, component_text* _text)
{
    g_drawport->SetFont(&_text->txt_fontdata);
    g_drawport->SetTextScaling(1.0f);
    g_drawport->SetTextAspect(1.0f);
    g_drawport->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    if (_text->txt_align == -1)
        g_drawport->PutText(_text->txt_str, box.Min()(1), box.Min()(2), _text->txt_color);
    else if (_text->txt_align == +1)
        g_drawport->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), _text->txt_color);
    else
        g_drawport->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), _text->txt_color);
}

void RenderSystem::render_button(component_position* _position, component_text* _text, component_button* _button, component_mousefocus* _mousefocus)
{
    g_drawport->SetFont(&_text->txt_fontdata);
    g_drawport->SetTextScaling(1.0f);
    g_drawport->SetTextAspect(1.0f);
    g_drawport->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    COLOR col;
    if (_mousefocus->mf_focus)
        col = _button->btn_color2;
    else
        col = _text->txt_color;

    if (_text->txt_align == -1)
        g_drawport->PutText(_text->txt_str, box.Min()(1), box.Min()(2), col);
    else if (_text->txt_align == +1)
        g_drawport->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), col);
    else
        g_drawport->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), col);
}

void RenderSystem::render_border(component_position* _position)
{
    g_drawport->DrawBorder(_position->pos_x, _position->pos_y, _position->pos_w, _position->pos_h, g_fb_color);
}

void RenderSystem::render_id(SEEntity* _entity, component_position* _position)
{
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));
    //g_drawport->SetFont(main_font_small);
    char* buffer = new char[32];
    snprintf(buffer, 32, "%i", _entity->id);
    g_drawport->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
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
        FLOAT2D(0.0f, 0.0f), FLOAT2D((float)g_drawport->GetWidth(), (float)g_drawport->GetHeight()));
    prPerspectiveProjection.AspectRatioL() = 1.0f;
    prPerspectiveProjection.FrontClipDistanceL() = 0.3f;

    CAnyProjection3D prProjection;
    prProjection = prPerspectiveProjection;

    // set up viewer position
    prProjection->ViewerPlacementL() = plCamera;
    // render the view
    RenderView(*g_world_data, *(CEntity*)NULL, prProjection, *g_drawport);
}

void RenderSystem::destroy_window(component_window* _window)
{
    if (g_viewport != NULL) {
        _pGfx->DestroyWindowCanvas(g_viewport);
        g_viewport = NULL;
    }
    // if window exists
    if (_window->win_pointer != NULL) {
        // destroy it
        SDL_DestroyWindow((SDL_Window*)_window->win_pointer);
        _window->win_pointer = NULL;
    }
}