#ifndef SSMF_CONTROLSYSTEM_H
#define SSMF_CONTROLSYSTEM_H

#include "System.h"

class ControlSystem : SESystem
{
public:
    void update() override;

    void control_button(SEActionComponent* _action, SEMouseFocusComponent* _mousefocus, SEMouseClickComponent* _mouseclick);
    void control_keyboard(SEActionComponent* _action, SEKeyboardComponent* _keyboard);
};



#endif