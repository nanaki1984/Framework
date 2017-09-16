#include "Core/Time/TimeServer.h"
#include "Core/Collections/Array.h"
#include "Core/Memory/MallocAllocator.h"

namespace Framework {

DefineClassInfo(Framework::TimeServer, Framework::RefCounted);

TimeServer::TimeServer()
: paused(false),
  realTime(0.0f),
  deltaTime(0.0f),
  totalTime(0.0f),
  totalPauseTime(0.0f),
  sources(Memory::GetAllocator<MallocAllocator>())
{
#if defined _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timeStart);
    QueryPerformanceFrequency((LARGE_INTEGER*)&timeFreq);
#elif defined __APPLE__
	machAbsTimeStart = mach_absolute_time();
	mach_timebase_info(&timebaseInfo);
#endif
}

TimeServer::~TimeServer()
{ }

void
TimeServer::AddSource(const SmartPtr<TimeSource> &timeSource)
{
    if (paused)
        timeSource->Pause();

    sources.PushBack(timeSource);
}

void
TimeServer::RemoveSource(const SmartPtr<TimeSource> &timeSource)
{
    sources.Remove(timeSource);
}

void
TimeServer::Tick()
{
    float oldTotalTime = totalTime;

#if defined _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timeCounter);

    realTime = ((timeCounter - timeStart) / (float)timeFreq);
#elif defined __APPLE__
    machAbsTime = mach_absolute_time();
    uint64_t nanosec = ((machAbsTime - machAbsTimeStart) * timebaseInfo.numer) / timebaseInfo.denom;

    realTime = nanosec * 0.000000001f;
#endif

    totalTime = paused ? oldTotalTime : (realTime - totalPauseTime);
    deltaTime = totalTime - oldTotalTime;

    auto it = sources.Begin(), end = sources.End();
    for (; it != end; ++it)
        (*it)->Update(deltaTime);
}

void
TimeServer::Pause()
{
    if (paused)
        return;

    paused = true;

#if defined _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timePaused);
#elif defined __APPLE__
	machAbsTimePaused = mach_absolute_time();
#endif

    auto it = sources.Begin(), end = sources.End();
    for (; it != end; ++it)
        (*it)->Pause();
}

void
TimeServer::Resume()
{
    if (!paused)
        return;

    paused = false;

#if defined _WIN32
	__int64 endTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&endTime);

    totalPauseTime += ((endTime - timePaused) / (float)timeFreq);
#elif defined __APPLE__
	uint64_t endTime = mach_absolute_time();
	uint64_t nanosec = ((endTime - machAbsTimePaused) * timebaseInfo.numer) / timebaseInfo.denom;
	
	totalPauseTime += (nanosec * 0.000000001f);
#endif

    auto it = sources.Begin(), end = sources.End();
    for (; it != end; ++it)
        (*it)->Play();
}

float
TimeServer::GetMilliseconds() const
{
#if defined _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timeCounter);

    return ((timeCounter - timeStart) * 1000 / (float)timeFreq);
#elif defined __APPLE__
    uint64_t absTime = mach_absolute_time();
    uint64_t nanosec = ((absTime - machAbsTimeStart) * timebaseInfo.numer) / timebaseInfo.denom;
    return nanosec * 0.000001f;
#endif
}

} // namespace Framework
