#ifndef SSMF_SYSTEM_H
#define SSMF_SYSTEM_H

#include <Engine/Base/Types.h>
#include <Engine/Engine.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Font.h>
#include "Entity.h"

class SESystem
{
    public:
    virtual void init(){};
    virtual void update(){};
};

#endif