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
#include <ECS/Manager.h>

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
extern UINT g_resolution_width;
extern UINT g_resolution_height;

void RenderSystem::preupdate()
{
    //TODO: What's the sense of calculate Milliseconds with a signed int???
    tloop1 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    if (!dp->Lock()) {
        //FIXME: Add some stop render flag.
        return;
    }
    // clear z-buffer
    dp->FillZBuffer(ZBUF_BACK);
    dp->Fill(C_BLACK | 0xff);
}

void RenderSystem::postupdate()
{
    dp->Unlock();
    vp->SwapBuffers();
    tloop2 = _pTimer->GetHighPrecisionTimer().GetMilliseconds();
    //TODO: Maybe do this 1 time at second, or render the value 1 time at second.
    if (tloop2 - tloop1)
        count_fps = 1000 / ((FLOAT)(tloop2 - tloop1));
    else
        count_fps = 0.0f;
}

void RenderSystem::updatePosition(component_camera* _camera)
{
    PIXaabbox2D box(PIX2D(0, 0), PIX2D(200, 100));
    //dp->SetFont(main_font_small);
    char* buffer = new char[128];
    float px = _camera->cam_pos(1);
    float py = _camera->cam_pos(2);
    float pz = _camera->cam_pos(3);
    float ax = _camera->cam_rot(1);
    float ay = _camera->cam_rot(2);
    float az = _camera->cam_rot(3);

    snprintf(buffer, 128, "X: %f\nY: %f\nZ: %f\nAX: %f \nAY: %f \nAZ: %f", px, py, pz, ax, ay, az);
    dp->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::updateFps()
{
    PIXaabbox2D box(PIX2D(0, 100), PIX2D(100, 100));
    //dp->SetFont(main_font_medium);
    char* buffer = new char[16];
    snprintf(buffer, 16, "FPS: %f", count_fps);
    dp->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::init(SEEntity* entity)
{
    component_window* window = dynamic_cast<component_window*>((SEEntity*)entity);
    if (window)
        initWindow(window);
}

void RenderSystem::initWindow(component_window* _window)
{
    if (_window->win_pointer != NULL)
        destroyWindow(_window);

    // try to set new display mode
    _pGfx->SetDisplayMode(_window->win_api, _window->win_adapter,
        g_resolution_width, g_resolution_height, _window->win_depth);

    ULONG tmp_flags = _window->win_flags;

    if (_window->win_api == GAT_OGL && !(tmp_flags & SDL_WINDOW_OPENGL))
        tmp_flags = tmp_flags | SDL_WINDOW_OPENGL;

    _window->win_pointer = SDL_CreateWindow(_window->win_title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        g_resolution_width, g_resolution_height, tmp_flags);

    if (_window->win_pointer == NULL)
        FatalError(TRANS("Cannot open main window!"));

    SE_UpdateWindowHandle(_window->win_pointer);

    _pGfx->CreateWindowCanvas(_window->win_pointer, &vp, &dp);
#ifndef DEBUG
    SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    // initial screen fill and swap, just to get context running
    if (dp != NULL && dp->Lock()) {
        dp->Fill(C_BLACK | 255);
        dp->Unlock();
        vp->SwapBuffers();
    }
}

void RenderSystem::update(SEEntity* entity)
{
    component_camera* camera = dynamic_cast<component_camera*>((SEEntity*)entity);
    if (camera)
        updateWorld(camera);

    component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
    if (g_dbg_draw_border && position)
        updateBorder(position);
    if (g_dbg_draw_id && position)
        updateId(entity, position);
    if (g_dbg_draw_position && camera)
        updatePosition(camera);
    if (g_dbg_draw_fps)
        updateFps();

    component_texture* texture = dynamic_cast<component_texture*>((SEEntity*)entity);
    if (position && texture)
        updateTexture(position, texture);
    component_mouse* mouse = dynamic_cast<component_mouse*>((SEEntity*)entity);
    if (mouse && g_dbg_draw_cursor)
        updateMousePos(mouse);
    if (mouse && texture)
        updateCursor(mouse, texture);

    component_text* text = dynamic_cast<component_text*>((SEEntity*)entity);
    component_mousefocus* mousefocus = dynamic_cast<component_mousefocus*>((SEEntity*)entity);
    component_button* button = dynamic_cast<component_button*>((SEEntity*)entity);
    if (position && text && button && mousefocus)
        updateButton(position, text, button, mousefocus);
    else if (position && text)
        updateText(position, text);
}

void RenderSystem::updateMousePos(component_mouse* _mouse)
{
    dp->DrawBorder(_mouse->mouse_cursor.x, _mouse->mouse_cursor.y,
        32, 32, g_fb_color);
}

void RenderSystem::updateCursor(component_mouse* _mouse, component_texture* _texture)
{
    dp->PutTexture(&_texture->tex_data,
        PIXaabbox2D(PIX2D(_mouse->mouse_cursor.x, _mouse->mouse_cursor.y),
            PIX2D(_mouse->mouse_cursor.x + 32, _mouse->mouse_cursor.y + 32)));
}

void RenderSystem::updateTexture(component_position* _position, component_texture* _texture)
{
    dp->PutTexture(&_texture->tex_data, PIXaabbox2D(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h)));
}

void RenderSystem::updateText(component_position* _position, component_text* _text)
{
    dp->SetFont(&_text->txt_fontdata);
    dp->SetTextScaling(1.0f);
    dp->SetTextAspect(1.0f);
    dp->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    if (_text->txt_align == -1)
        dp->PutText(_text->txt_str, box.Min()(1), box.Min()(2), _text->txt_color);
    else if (_text->txt_align == +1)
        dp->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), _text->txt_color);
    else
        dp->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), _text->txt_color);
}

void RenderSystem::updateButton(component_position* _position, component_text* _text, component_button* _button, component_mousefocus* _mousefocus)
{
    dp->SetFont(&_text->txt_fontdata);
    dp->SetTextScaling(1.0f);
    dp->SetTextAspect(1.0f);
    dp->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    COLOR col;
    if (_mousefocus->mf_focus)
        col = _button->btn_color2;
    else
        col = _text->txt_color;

    if (_text->txt_align == -1)
        dp->PutText(_text->txt_str, box.Min()(1), box.Min()(2), col);
    else if (_text->txt_align == +1)
        dp->PutTextR(_text->txt_str, box.Max()(1), box.Min()(2), col);
    else
        dp->PutTextC(_text->txt_str, box.Center()(1), box.Min()(2), col);
}

void RenderSystem::updateBorder(component_position* _position)
{
    dp->DrawBorder(_position->pos_x, _position->pos_y, _position->pos_w, _position->pos_h, g_fb_color);
}

void RenderSystem::updateId(SEEntity* _entity, component_position* _position)
{
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));
    //dp->SetFont(main_font_small);
    char* buffer = new char[32];
    snprintf(buffer, 32, "%i", _entity->id);
    dp->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::updateWorld(component_camera* _camera)
{
    CPlacement3D plCamera;
    plCamera.pl_PositionVector = _camera->cam_pos;
    plCamera.pl_OrientationAngle = _camera->cam_rot;

    // init projection parameters
    CPerspectiveProjection3D prPerspectiveProjection;
    prPerspectiveProjection.FOVL() = _camera->cam_fov;
    prPerspectiveProjection.ScreenBBoxL() = FLOATaabbox2D(
        FLOAT2D(0.0f, 0.0f), FLOAT2D((float)dp->GetWidth(), (float)dp->GetHeight()));
    prPerspectiveProjection.AspectRatioL() = 1.0f;
    prPerspectiveProjection.FrontClipDistanceL() = 0.3f;

    CAnyProjection3D prProjection;
    prProjection = prPerspectiveProjection;

    // set up viewer position
    prProjection->ViewerPlacementL() = plCamera;
    // render the view
    RenderView(*g_world_data, *(CEntity*)NULL, prProjection, *dp);
}

void RenderSystem::destroyWindow(component_window* _window)
{
    if (vp != NULL) {
        _pGfx->DestroyWindowCanvas(vp);
        vp = NULL;
    }
    // if window exists
    if (_window->win_pointer != NULL) {
        // destroy it
        SDL_DestroyWindow((SDL_Window*)_window->win_pointer);
        _window->win_pointer = NULL;
    }
}
