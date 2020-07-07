#ifndef SSMF_OBJECT_H
#define SSMF_OBJECT_H

#include "Render.h" /* CDrawPort */

class SEObject {
public:
    SEObject();
    SEObject(CDrawPort* drawPort);
    ~SEObject();

    virtual void render(const SERender* render) {};
    virtual void update() {};
    UINT size();
};

#endif