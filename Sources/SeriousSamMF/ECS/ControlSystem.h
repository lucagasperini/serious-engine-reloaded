#ifndef SSMF_CONTROLSYSTEM_H
#define SSMF_CONTROLSYSTEM_H

#include "Entity.h"

class ControlSystem {
public:
    void init(SEEntity* entity);
    void update(SEEntity* entity);

    void control_button(SEActionComponent* _action, SEMouseFocusComponent* _mousefocus, SEMouseClickComponent* _mouseclick);
    void control_keyboard(SEActionComponent* _action, SEKeyboardComponent* _keyboard);
    void control_camera(SECameraComponent* _camera, SEMouseDeltaComponent* _mousedelta, SEKeybindComponent* _keybind);
    void control_game(SEKeybindComponent* _keybind);
};

#endif