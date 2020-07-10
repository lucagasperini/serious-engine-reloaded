#include "ControlSystem.h"
#include "Manager.h"

extern ECSManager* manager;

void ControlSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*manager->entities, SEEntity, entity)
    {
        SEKeyboardComponent* keyboard = dynamic_cast<SEKeyboardComponent *>((SEEntity*)entity);
        SEActionComponent* action = dynamic_cast<SEActionComponent *>((SEEntity*)entity);
        SEMouseFocusComponent* mousefocus = dynamic_cast<SEMouseFocusComponent *>((SEEntity*)entity);
        SEMouseClickComponent* mouseclick = dynamic_cast<SEMouseClickComponent *>((SEEntity*)entity);

        if(mouseclick && mousefocus && action)
            control_button(action, mousefocus, mouseclick);
        if(keyboard && action)
            control_keyboard(action, keyboard);
    }
}

void ControlSystem::control_button(SEActionComponent* _action, SEMouseFocusComponent* _mousefocus, SEMouseClickComponent* _mouseclick)
{
    if(_mousefocus->semf_focus && _mouseclick->semc_button == SDL_BUTTON_LEFT)
        _action->sea_action();
}

void ControlSystem::control_keyboard(SEActionComponent* _action, SEKeyboardComponent* _keyboard)
{
    if(_keyboard->sekc_key == _keyboard->sekc_listen_key)
        _action->sea_action();
}