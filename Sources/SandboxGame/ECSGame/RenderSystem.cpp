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
#include <SER/ECS/Manager.h>

#include <Engine/Templates/Stock_CTextureData.h>

using namespace SER;

//TODO: Create a structure to manage fonts.
//extern CFontData* main_font_small;
//extern CFontData* main_font_medium;
extern COLOR g_fb_color;
extern CWorld* g_world_data;

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

void RenderSystem::updatePosition(ComponentCamera* _camera)
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

void RenderSystem::init(Entity* _entity)
{
    System::init(_entity);
}

void RenderSystem::update(Entity* _entity)
{
    System::update(_entity);

    SER_GET_COMPONENT(camera, ComponentCamera, _entity);
    if (camera)
        updateCamera(camera);

    SER_GET_COMPONENT(position, ComponentPosition, _entity);

    SER_GET_SETTING_ARG(debug_border, BOOL, SC_DEBUG_BORDER);
    SER_GET_SETTING_ARG(debug_position, BOOL, SC_DEBUG_POSITION);
    SER_GET_SETTING_ARG(debug_entity_id, BOOL, SC_DEBUG_ENTITYID);
    SER_GET_SETTING_ARG(debug_fps, BOOL, SC_DEBUG_FPS);

    if (*debug_border && position)
        updateBorder(position);
    if (*debug_entity_id && position)
        updateId(position);
    if (*debug_position && camera)
        updatePosition(camera);
    if (*debug_fps)
        updateFps();

    SER_GET_COMPONENT(texture, ComponentTexture, _entity);
    if (position && texture)
        updateTexture(position, texture);

    SER_GET_COMPONENT(button, ComponentButton, _entity);
    if (position && button)
        updateButton(position, button);

    SER_GET_COMPONENT(cursor, ComponentCursor, _entity);
    if (cursor)
        updateCursor(cursor);
}

void RenderSystem::updateCursor(ComponentCursor* _cursor)
{
    SER_GET_SETTING_ARG(debug_cursor, BOOL, SC_DEBUG_CURSOR);
    if (*debug_cursor)
        dp->DrawBorder(_cursor->x, _cursor->y, _cursor->w, _cursor->h, g_fb_color);

    CTextureObject* tmp_obj = new CTextureObject;
    tmp_obj->SetData_t(_cursor->texture);
    dp->PutTexture(tmp_obj,
        PIXaabbox2D(PIX2D(_cursor->x, _cursor->y),
            PIX2D(_cursor->x + _cursor->w, _cursor->y + _cursor->h)));
}

void RenderSystem::updateTexture(ComponentPosition* _position, ComponentTexture* _texture)
{
    CTextureObject* tmp_obj = new CTextureObject;
    tmp_obj->SetData_t(_texture->tex_file);
    dp->PutTexture(tmp_obj, PIXaabbox2D(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h)));
}

void RenderSystem::updateButton(ComponentPosition* _position, ComponentButton* _button)
{
    CFontData font_big;
    font_big.Load_t(CTFILENAME("Fonts\\Display3-caps.fnt"));
    dp->SetFont(&font_big);
    dp->SetTextScaling(1.0f);
    dp->SetTextAspect(1.0f);
    //dp->SetTextMode(_text->txt_mode);
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y),
        PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));

    COLOR col;
    SER_GET_EVENT_ARG(focus, ULONG, EC_BUTTON_ONFOCUS);
    if (focus && *focus == this_entity->id) {
        SER_REMOVE_EVENT(EC_BUTTON_ONFOCUS);
        col = _button->color_focus;
    } else {
        col = _button->color;
    }
    if (_button->align == -1)
        dp->PutText(_button->text, box.Min()(1), box.Min()(2), col);
    else if (_button->align == +1)
        dp->PutTextR(_button->text, box.Max()(1), box.Min()(2), col);
    else
        dp->PutTextC(_button->text, box.Center()(1), box.Min()(2), col);
}

void RenderSystem::updateBorder(ComponentPosition* _position)
{
    dp->DrawBorder(_position->pos_x, _position->pos_y, _position->pos_w, _position->pos_h, g_fb_color);
}

void RenderSystem::updateId(ComponentPosition* _position)
{
    PIXaabbox2D box(PIX2D(_position->pos_x, _position->pos_y), PIX2D(_position->pos_x + _position->pos_w, _position->pos_y + _position->pos_h));
    //dp->SetFont(main_font_small);
    char* buffer = new char[32];
    snprintf(buffer, 32, "%i", this_entity->id);
    dp->PutText(buffer, box.Min()(1), box.Min()(2), g_fb_color);
}

void RenderSystem::updateCamera(ComponentCamera* _camera)
{
    CPlacement3D placement;
    placement.pl_PositionVector = _camera->cam_pos;
    placement.pl_OrientationAngle = _camera->cam_rot;

    // init projection parameters
    CPerspectiveProjection3D perspective;
    perspective.FOVL() = _camera->cam_fov;
    perspective.ScreenBBoxL() = FLOATaabbox2D(
        FLOAT2D(0.0f, 0.0f), FLOAT2D((FLOAT)dp->GetWidth(), (FLOAT)dp->GetHeight()));
    perspective.AspectRatioL() = 1.0f;
    perspective.FrontClipDistanceL() = 0.3f;

    CAnyProjection3D projection;
    projection = perspective;

    // set up viewer position
    projection->ViewerPlacementL() = placement;
    // render the view
    RenderView(*g_world_data, *(CEntity*)NULL, projection, *dp);
}
