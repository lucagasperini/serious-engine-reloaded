#ifndef SSMF_INPUTSYSTEM_H
#define SSMF_INPUTSYSTEM_H

#include <Engine/Base/Types.h>
#include "Entity.h"

class InputSystem
{
    public:
    FLOAT sensibility = 0.25;
    POINT* old_cursor = NULL;
    ULONG key;
    ULONG button;
    POINT cursor;
    POINT deltacursor;
    SDL_Event event;

    void preupdate();
    void postupdate();
    void init(SEEntity* entity);
    void update(SEEntity* entity);
};

#endif