#ifndef SSMF_POSITIONSYSTEM_H
#define SSMF_POSITIONSYSTEM_H

#include "Entity.h"

class PositionSystem {
public:
    void init(SEEntity* entity);
    void postinit();
    void init_scale(SEPositionComponent* _position);
    void init_align(SEPositionComponent* _position, SEAlignComponent* _align);

    void update(SEEntity* entity);
    void postupdate();
};

#endif