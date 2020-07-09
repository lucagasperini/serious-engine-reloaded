#ifndef SSMF_SYSTEM_H
#define SSMF_SYSTEM_H

#include <Engine/Base/Types.h>
#include <Engine/Engine.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Font.h>
#include "Entity.h"

extern CDrawPort* main_dp;
extern CDynamicContainer<SEEntity>* pEntities;

class SESystem
{
    public:
    virtual void init() {}
    virtual void update() {}
};


class ScalePositionSystem : SESystem
{
    public:
    ULONG old_width = 1920;
    ULONG old_height = 1080;

    void init();
};

class AlignPositionSystem : SESystem
{
    public:
    void init();
};

class TextureRenderSystem : SESystem
{
    public:
    void init();
    void update();
};

class ButtonRenderSystem : SESystem
{
    public:

    BOOL focus;
    void update();
};

class BorderRenderSystem : SESystem
{
    void update();
};


#endif