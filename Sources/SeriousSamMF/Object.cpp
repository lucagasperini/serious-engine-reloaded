#include "Object.h"

#include <Engine/Engine.h>

SEObject::SEObject()
{

}

SEObject::~SEObject()
{
    
}

UINT SEObject::size()
{
    return sizeof(this);
}