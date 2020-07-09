#ifndef SSMF_INPUTSYSTEM_H
#define SSMF_INPUTSYSTEM_H

#include "System.h"

class InputSystem : SESystem
{
    public:

    void update() override;
    void input_button(SEPositionComponent* _position, SEButtonComponent* _button);
};

#endif