#ifndef SSMF_MANAGER_H
#define SSMF_MANAGER_H

#include <Engine/Engine.h>
#include "Entity.h"
#include "System.h"

class ECSManager
{
    private:
    static ULONG entity_counter;
    public:
    CDynamicContainer<SEEntity>* entities = NULL;
    CDynamicContainer<SESystem>* systems = NULL;

    ECSManager();
    ~ECSManager();

    void addEntity(SEEntity* _entity);
    void addSystem(SESystem* _system);
};

#endif