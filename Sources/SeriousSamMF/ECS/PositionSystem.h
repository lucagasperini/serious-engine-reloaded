#ifndef SSMF_POSITIONSYSTEM_H
#define SSMF_POSITIONSYSTEM_H

#include "System.h"

class PositionSystem : SESystem
{
    public:
    ULONG old_width = 1920;
    ULONG old_height = 1080;

    void init() override;
    void init_scale(SEPositionComponent* _position, ULONG width, ULONG height);
    void init_align(SEPositionComponent* _position, SEAlignComponent* _align);

    void update();
};

#endif