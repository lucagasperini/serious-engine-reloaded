#include "InputSystem.h"

extern BOOL runningLevel;
extern BOOL main_game_started;
extern int SE_SDL_InputEventPoll(SDL_Event *event);

void InputSystem::init(SEEntity* entity)
{

}

void InputSystem::preupdate()
{
        // get real cursor position
    GetCursorPos(&cursor);
    if (old_cursor != NULL)
    {
        deltacursor.x = (old_cursor->x - cursor.x) * sensibility;
        deltacursor.y = (old_cursor->y - cursor.y) * sensibility;
    }
    else
    {
        old_cursor = new POINT();
    }
    old_cursor->x = cursor.x;
    old_cursor->y = cursor.y;

    while (SE_SDL_InputEventPoll(&event))
    {
        if (event.type == SDL_QUIT)
        {
            main_game_started = FALSE;
        }
        if (event.type == SDL_KEYDOWN)
        {
            key = event.key.keysym.sym;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            button = event.button.button;
        }
    }
}

void InputSystem::postupdate()
{
    key = 0;
    button = 0;
    cursor = POINT();
    deltacursor = POINT();
}

void InputSystem::update(SEEntity* entity)
{
    SEKeyboardComponent *keyboard = dynamic_cast<SEKeyboardComponent *>((SEEntity *)entity);
    if (keyboard)
        keyboard->kc_listen_key = key;
    SEKeybindComponent *keybind = dynamic_cast<SEKeybindComponent *>((SEEntity *)entity);
    if (keybind)
    {
        // Reset current keybind first
        keybind->kb_current = SE_KEYBIND_NULL;
        for (ULONG i = 1; i < SE_ECS_KEYBIND_MAX; i++)
        {
            if (keybind->kb_keybind[i] == key && key != 0)
            {
                keybind->kb_current = i;
                break;
            }
        }
    }

    SEPositionComponent *position = dynamic_cast<SEPositionComponent *>((SEEntity *)entity);
    SEMouseFocusComponent *mousefocus = dynamic_cast<SEMouseFocusComponent *>((SEEntity *)entity);
    if (position && mousefocus)
    {
        if (position->pos_x < cursor.x &&
            position->pos_y < cursor.y &&
            position->pos_x + position->pos_w > cursor.x &&
            position->pos_y + position->pos_h > cursor.y)
        {
            mousefocus->mf_focus = TRUE;
        }
        else
        {
            mousefocus->mf_focus = FALSE;
        }
    }

    SEMouseClickComponent *mouseclick = dynamic_cast<SEMouseClickComponent *>((SEEntity *)entity);
    if (mouseclick)
        mouseclick->mc_button = button;

    SEMouseDeltaComponent *mousedelta = dynamic_cast<SEMouseDeltaComponent *>((SEEntity *)entity);
    if (mousedelta)
    {
        mousedelta->md_cursor.x = deltacursor.x;
        mousedelta->md_cursor.y = deltacursor.y;
    }
}