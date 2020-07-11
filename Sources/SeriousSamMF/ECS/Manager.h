#ifndef SSMF_MANAGER_H
#define SSMF_MANAGER_H

#include <Engine/Engine.h>
#include "Entity.h"
#include "RenderSystem.h"
#include "PositionSystem.h"
#include "InputSystem.h"
#include "ControlSystem.h"


class ECSManager
{
    private:
    static ULONG entity_counter;
    public:
    PositionSystem* position_system = NULL;
    RenderSystem* render_system = NULL;
    InputSystem* input_system = NULL;
    ControlSystem* control_system = NULL;
    CDynamicContainer<SEEntity>* entities = NULL;

    ECSManager();
    ~ECSManager();

    void init();
    void update();

    void addEntity(SEEntity* _entity);
};

#endif