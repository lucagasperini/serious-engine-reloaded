#ifndef SSMF_INPUTSYSTEM_H
#define SSMF_INPUTSYSTEM_H

#include "System.h"

class InputSystem : SESystem
{
    public:
    FLOAT sensibility = 0.25;
    POINT* old_cursor = NULL;

    void update() override;
};

#endif