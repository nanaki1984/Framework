#include "Core/Time/TimeSource.h"
#include "Core/Debug.h"

namespace Framework {

DefineClassInfo(Framework::TimeSource, Framework::RefCounted);

TimeSource::TimeSource()
: pauseCounter(0),
  deltaTime(0.0f),
  timeFactor(1.0f),
  totalTime(0.0f)
{ }

TimeSource::~TimeSource()
{ }

void
TimeSource::Play()
{
    assert(pauseCounter > 0);
    --pauseCounter;
}

void
TimeSource::Pause()
{
    ++pauseCounter;
}

void
TimeSource::Reset()
{
    totalTime = 0.0f;
}

void
TimeSource::Update(float dt)
{
    deltaTime = (0 == pauseCounter ? dt * timeFactor : 0.0f);
    totalTime += deltaTime;
}

} // namespace Framework
