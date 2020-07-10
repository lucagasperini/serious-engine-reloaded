#include "PositionSystem.h"
#include "Manager.h"

extern ECSManager* manager;
extern CDrawPort* main_dp;

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
    if(old_width != main_dp->GetWidth() || old_height != main_dp->GetHeight())
        init();
}