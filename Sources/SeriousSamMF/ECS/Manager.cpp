#include "Manager.h"

ULONG ECSManager::entity_counter = 0;

ECSManager::ECSManager()
{
    entities = new CDynamicContainer<SEEntity>;
    systems = new CDynamicContainer<SESystem>;
}

ECSManager::~ECSManager()
{

}

void ECSManager::addEntity(SEEntity* _entity)
{
    _entity->id = entity_counter++;
    entities->Add(_entity);
}

void ECSManager::addSystem(SESystem* _system)
{
    systems->Add(_system);
}