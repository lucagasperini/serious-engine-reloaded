#ifndef SSMF_ENTITY_H
#define SSMF_ENTITY_H

#include "Component.h"

struct SEEntity {
    ULONG id;
    virtual ~SEEntity() {}
};

struct MenuImage : SEEntity,
                   SETextureComponent,
                   SEPositionComponent {
};

struct MenuButton : SEEntity,
                    SETextComponent,
                    SEPositionComponent,
                    SEAlignComponent,
                    SEButtonComponent,
                    SEMouseClickComponent,
                    SEMouseFocusComponent,
                    SEActionComponent {
};
struct GameControl : SEEntity,
                     SEKeyboardComponent,
                     SEKeybindComponent {
};

struct Camera : SEEntity,
                SEKeybindComponent,
                SEMouseDeltaComponent,
                SECameraComponent,
                SEActionComponent {
};

#endif