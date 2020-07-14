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

extern CDrawPort* main_dp;
extern UINT game_vresolution_width;
extern UINT game_vresolution_height;

void PositionSystem::init(SEEntity* entity)
{
    SEPositionComponent* position = dynamic_cast<SEPositionComponent*>((SEEntity*)entity);
}

void PositionSystem::postinit()
{
}

void PositionSystem::init_scale(SEPositionComponent* _position)
{
    FLOAT scaleX = (FLOAT)main_dp->GetWidth() / (FLOAT)game_vresolution_width;
    FLOAT scaleY = (FLOAT)main_dp->GetHeight() / (FLOAT)game_vresolution_height;

    _position->pos_x = (FLOAT)_position->pos_x * scaleX;
    _position->pos_y = (FLOAT)_position->pos_y * scaleY;
    _position->pos_w = (FLOAT)_position->pos_w * scaleX;
    _position->pos_h = (FLOAT)_position->pos_h * scaleY;
}

void PositionSystem::init_align(SEPositionComponent* _position, SEAlignComponent* _align)
{
    ULONG center_x = main_dp->GetWidth() / 2;
    ULONG center_y = main_dp->GetHeight() / 2;

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
    if (game_vresolution_width != main_dp->GetWidth() || game_vresolution_height != main_dp->GetHeight()) {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent*>((SEEntity*)entity);
        SEAlignComponent* align = dynamic_cast<SEAlignComponent*>((SEEntity*)entity);

        if (position)
            init_scale(position);

        if (position && align)
            init_align(position, align);
    }
}

void PositionSystem::postupdate()
{
    if (game_vresolution_width != main_dp->GetWidth() || game_vresolution_height != main_dp->GetHeight()) {
        game_vresolution_width = main_dp->GetWidth();
        game_vresolution_height = main_dp->GetHeight();
    }
}