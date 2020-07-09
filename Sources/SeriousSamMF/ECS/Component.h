#ifndef SSMF_COMPONENT_H
#define SSMF_COMPONENT_H


#include <Engine/Base/Types.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Graphics/Texture.h>

struct SEComponent
{
};


struct SEPositionComponent : SEComponent
{
    public:
    ULONG w, h;
    ULONG x, y;
};

struct SETextureComponent : SEComponent
{
    public:
    CTextureObject tex;
    CTFileName fntex;
};

struct SEFontComponent : SEComponent
{
    public:
    INDEX fontsize;
    INDEX align;
    CFontData fontdata;
    INDEX textmode;
    COLOR color;
    COLOR color2;
};

struct SEButtonComponent : SEComponent
{
    public:
    void (*function)();
};

struct SETextComponent : SEComponent
{
    public:
    CTString str;
};

struct SEAlignComponent : SEComponent
{
    public:
    /*
    -2 => Left
    -1 => Center Left
    0  => Center
    1  => Center Right
    2  => Right 
    */
    INDEX align_x = 64;
    /*
    -2 => Bottom
    -1 => Center Bottom
    0  => Center
    1  => Center Top
    2  => Top 
    */
    INDEX align_y = 64;
};

#endif