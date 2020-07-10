#include "InputSystem.h"
#include "Manager.h"

extern ECSManager* manager;

extern BOOL runningLevel;
extern BOOL main_game_started;
extern int SE_SDL_InputEventPoll(SDL_Event *event);

void InputSystem::update()
{
    ULONG key;
    ULONG button;
    POINT cursor;
    POINT deltacursor;
    SDL_Event event;
    // get real cursor position
    GetCursorPos(&cursor);
    if(old_cursor != NULL) {
        deltacursor.x = (old_cursor->x - cursor.x) * sensibility;
        deltacursor.y = (old_cursor->y - cursor.y) * sensibility;
    } else {
        old_cursor = new POINT();
    }
        //pen->en_plPlacement.Rotate_Airplane(ANGLE3D(delta.x, delta.y, 0.0f));
    old_cursor->x = cursor.x;
    old_cursor->y = cursor.y;
    while (SE_SDL_InputEventPoll(&event)) {
        if(event.type == SDL_QUIT) {
            main_game_started = FALSE;
        }
        if(event.type == SDL_KEYDOWN) {
            key = event.key.keysym.sym;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            button = event.button.button;
        }
    }
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SEKeyboardComponent* keyboard = dynamic_cast<SEKeyboardComponent *>((SEEntity*)entity);
        if(keyboard)
            keyboard->sekc_listen_key = key;

        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        SEMouseFocusComponent* mousefocus = dynamic_cast<SEMouseFocusComponent *>((SEEntity*)entity);
        if (position && mousefocus)
        {
            if (position->x < cursor.x &&
                position->y < cursor.y &&
                position->x + position->w > cursor.x &&
                position->y + position->h > cursor.y) {
                mousefocus->semf_focus = TRUE;
            }
            else
            {
                mousefocus->semf_focus = FALSE;
            }
            
        }

        SEMouseClickComponent* mouseclick = dynamic_cast<SEMouseClickComponent *>((SEEntity*)entity);
        if (mouseclick)
            mouseclick->semc_button = button;
    }
}