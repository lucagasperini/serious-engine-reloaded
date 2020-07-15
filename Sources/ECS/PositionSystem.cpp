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

#include "PositionSystem.h"

extern CDrawPort* g_drawport;
extern UINT g_vresolution_width;
extern UINT g_vresolution_height;

void PositionSystem::init_scale(component_position* _position)
{
    FLOAT scaleX = (FLOAT)g_drawport->GetWidth() / (FLOAT)g_vresolution_width;
    FLOAT scaleY = (FLOAT)g_drawport->GetHeight() / (FLOAT)g_vresolution_height;

    _position->pos_x = (FLOAT)_position->pos_x * scaleX;
    _position->pos_y = (FLOAT)_position->pos_y * scaleY;
    _position->pos_w = (FLOAT)_position->pos_w * scaleX;
    _position->pos_h = (FLOAT)_position->pos_h * scaleY;
}

void PositionSystem::init_align(component_position* _position, component_align* _align)
{
    ULONG center_x = g_drawport->GetWidth() / 2;
    ULONG center_y = g_drawport->GetHeight() / 2;

    switch (_align->align_x) {
    case -2:
        _position->pos_x = 0;
        break;
    case -1:
        _position->pos_x = center_x - _position->pos_w;
        break;
    case 0:
        _position->pos_x = center_x - _position->pos_w / 2;
        break;
    case 1:
        _position->pos_x = center_x;
        break;
    case 2:
        _position->pos_x = center_x * 2 - _position->pos_w;
        break;
    }
    switch (_align->align_y) {
    case -2:
        _position->pos_y = 0;
        break;
    case -1:
        _position->pos_y = center_y - _position->pos_h;
        break;
    case 0:
        _position->pos_y = center_y - _position->pos_h / 2;
        break;
    case 1:
        _position->pos_y = center_y;
        break;
    case 2:
        _position->pos_y = center_y * 2 - _position->pos_y;
        break;
    }
}

void PositionSystem::update(SEEntity* entity)
{
    if (g_vresolution_width != g_drawport->GetWidth() || g_vresolution_height != g_drawport->GetHeight()) {
        component_position* position = dynamic_cast<component_position*>((SEEntity*)entity);
        component_align* align = dynamic_cast<component_align*>((SEEntity*)entity);

        if (position)
            init_scale(position);

        if (position && align)
            init_align(position, align);
    }
}

void PositionSystem::postupdate()
{
    if (g_vresolution_width != g_drawport->GetWidth() || g_vresolution_height != g_drawport->GetHeight()) {
        g_vresolution_width = g_drawport->GetWidth();
        g_vresolution_height = g_drawport->GetHeight();
    }
}