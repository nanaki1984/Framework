#pragma once

#include "Core/Collections/Array_type.h"
#include "Core/Singleton.h"
#include "Core/Time/TimeSource.h"
#include "Core/SmartPtr.h"

#if defined _WIN32
#	define NOMINMAX
#   include <windows.h>
#elif defined __APPLE__
#   include <mach/mach_time.h>
#endif

namespace Framework {

class TimeServer : public Singleton<TimeServer> {
    DeclareClassInfo;
protected:
	bool paused;
	float realTime;
    float deltaTime;
    float totalTime;
    float totalPauseTime;

    Array<SmartPtr<TimeSource>> sources;

#if defined _WIN32
    __int64 timeStart;
    __int64 timeFreq;
    __int64 timeCounter;
	__int64 timePaused;
#elif defined __APPLE__
	mach_timebase_info_data_t timebaseInfo;
	uint64_t machAbsTimeStart;
	uint64_t machAbsTime;
	uint64_t machAbsTimePaused;
#elif defined EMSCRIPTEN
	double timeStart;
	double timePaused;
#endif
public:
    TimeServer();
    virtual ~TimeServer();

    void AddSource(const SmartPtr<TimeSource> &timeSource);
    void RemoveSource(const SmartPtr<TimeSource> &timeSource);

    void Tick();
    void Pause();
    void Resume();

    float GetMilliseconds() const;

    bool IsPaused() const;
    float GetRealTime() const;
    float GetDeltaTime() const;
    float GetTime() const;
};

inline bool
TimeServer::IsPaused() const
{
    return paused;
}

inline float
TimeServer::GetRealTime() const
{
    return realTime;
}

inline float
TimeServer::GetDeltaTime() const
{
    return deltaTime;
}

inline float
TimeServer::GetTime() const
{
    return totalTime;
}

} // namespace Framework
