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

extern UINT g_resolution_width;
extern UINT g_resolution_height;
extern UINT g_virtual_resolution_width;
extern UINT g_virtual_resolution_height;
extern BOOL g_event_current;

using namespace SER;

void PositionSystem::initScale(ComponentPosition* _position)
{
    FLOAT scaleX = (FLOAT)g_resolution_width / (FLOAT)g_virtual_resolution_width;
    FLOAT scaleY = (FLOAT)g_resolution_height / (FLOAT)g_virtual_resolution_height;

    _position->pos_x = (FLOAT)_position->pos_x * scaleX;
    _position->pos_y = (FLOAT)_position->pos_y * scaleY;
    _position->pos_w = (FLOAT)_position->pos_w * scaleX;
    _position->pos_h = (FLOAT)_position->pos_h * scaleY;
}

void PositionSystem::initAlign(ComponentPosition* _position, ComponentAlign* _align)
{
    ULONG center_x = g_resolution_width / 2;
    ULONG center_y = g_resolution_height / 2;

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

void PositionSystem::init(Entity* _entity)
{
    System::init(_entity);
    if (scale_x != g_resolution_width || scale_y != g_resolution_height) {
        SER_GET_COMPONENT(position, ComponentPosition, _entity);
        SER_GET_COMPONENT(align, ComponentAlign, _entity);

        if (position)
            initScale(position);

        if (position && align)
            initAlign(position, align);
    }
}

void PositionSystem::postinit()
{
    scale_x = g_resolution_width;
    scale_y = g_resolution_height;
}

void PositionSystem::update(Entity* _entity)
{
    System::update(_entity);
}
