#include "TestRotate.h"
#include "Core/Time/TimeServer.h"
#include "Game/Entity.h"
#include "Game/ComponentsList.h"
#include "Math/AxisAngle.h"
#include "Game/MsgDispatcher.h"
//#include "Core/Log.h"

DefineClassInfo(TestRotate, Framework::Component);
DefineComponent(TestRotate, 0);

using namespace Framework;
using namespace Framework::Math;

TestRotate::TestRotate()
: Component()
{ }

TestRotate::TestRotate(const TestRotate &other)
: Component(other),
  rotAxis(other.rotAxis),
  angle(other.angle)
{ }

TestRotate::TestRotate(TestRotate &&other)
: Component(std::forward<Component>(other)),
  rotAxis(other.rotAxis),
  angle(other.angle)
{ }

TestRotate::~TestRotate()
{
    MsgDispatcher<void(void)>::Instance()->Remove("MyMessageTest", this);
}

void
TestRotate::OnCreate()
{
    MsgDelegate<void(void)>::Create<TestRotate, &TestRotate::MyMessageTest>("MyMessageTest", this);

    rotAxis = Vector3(
        (rand() / (float)RAND_MAX) * 2.0f - 1.0f,
        (rand() / (float)RAND_MAX) * 2.0f - 1.0f,
        (rand() / (float)RAND_MAX) * 2.0f - 1.0f);
    rotAxis.Normalize();
    angle = 0.0f;
    deactivationTime = TimeServer::Instance()->GetTime() + 5.0f + (rand() / (float)RAND_MAX) * 10.0f;
}

void
TestRotate::Update()
{
    angle += TimeServer::Instance()->GetDeltaTime() * 180.0f;
    //Log::Instance()->Write(Log::Info, "%s %f\n", entity->GetTransform()->GetName(), angle);
    entity->GetTransform()->SetLocalRotation(AxisAngle(rotAxis, angle));
/*
    if (2 == GetInstanceID() && TimeServer::Instance()->GetTime() > 5.0f)
        entity->DestroyRecursively();*/
/*
    if (TimeServer::Instance()->GetTime() >= deactivationTime)
    {
        entity->SetActive(false);
        entity->SendMessage("MyMessageTest");
    }*/
}

void
TestRotate::MyMessageTest()
{
    Log::Instance()->Write(Log::Info, "MyMessageTest");
}
