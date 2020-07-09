#include "InputSystem.h"
#include "Manager.h"

extern ECSManager* manager;

extern POINT cursor;
extern SDL_Event* event;

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