#pragma once

#include "Game/Component.h"
#include "Math/Vector3.h"

class TestRotate : public Framework::Component {
    DeclareClassInfo;
    DeclareComponent;
protected:
    Framework::Math::Vector3 rotAxis;
    float angle;
    float deactivationTime;
public:
    TestRotate();
    TestRotate(const TestRotate &other);
    TestRotate(TestRotate &&other);
    virtual ~TestRotate();

    void OnCreate();
    void Update();

    void MyMessageTest();
};
