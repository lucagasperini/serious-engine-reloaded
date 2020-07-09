#ifndef SSMF_ENTITY_H
#define SSMF_ENTITY_H

#include "Component.h"

struct SEEntity
{
    ULONG id;
    virtual ~SEEntity() {}
};

struct MenuImage : SEEntity, SETextureComponent, SEPositionComponent{};

struct MenuButton : SEEntity, 
                    SEFontComponent, 
                    SETextComponent, 
                    SEPositionComponent, 
                    SEAlignComponent, 
                    SEButtonComponent{};

#endif