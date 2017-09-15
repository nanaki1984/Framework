#pragma once

#include "Game/Component.h"
#include "Components/Transform.h"

class CamInput : public Framework::Component {
    DeclareClassInfo;
    DeclareComponent;
public:
    CamInput();
    CamInput(const CamInput &other);
    CamInput(CamInput &&other);
    virtual ~CamInput();

    void Update();
};
