#include "Manager.h"

ULONG ECSManager::entity_counter = 0;

ECSManager::ECSManager()
{
    entities = new CDynamicContainer<SEEntity>;
    position_system = new PositionSystem;
    render_system = new RenderSystem;
    input_system = new InputSystem;
    control_system = new ControlSystem;
}

ECSManager::~ECSManager()
{

}

void ECSManager::addEntity(SEEntity* _entity)
{
    _entity->id = entity_counter++;
    entities->Add(_entity);
}

void ECSManager::init()
{
    FOREACHINDYNAMICCONTAINER(*entities, SEEntity, entity)
    {
        position_system->init(entity);
        input_system->init(entity);
        render_system->init(entity);
        control_system->init(entity);
    }
    position_system->postinit();
}

void ECSManager::update()
{
    input_system->preupdate();
    FOREACHINDYNAMICCONTAINER(*entities, SEEntity, entity)
    {
        input_system->update(entity);
        position_system->update(entity);
        render_system->update(entity);
        control_system->update(entity);
    }
    position_system->postupdate();
    input_system->postupdate();
}