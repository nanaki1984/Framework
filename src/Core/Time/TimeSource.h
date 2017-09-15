#pragma once

#include "Core/RefCounted.h"

namespace Framework {

class TimeSource : public RefCounted {
    DeclareClassInfo;
protected:
    unsigned int pauseCounter;

    float deltaTime;
    float timeFactor;
    float totalTime;
public:
    TimeSource();
    virtual ~TimeSource();

    bool IsMaster() const;
    bool IsPaused() const;

    void SetTimeFactor(float factor);
    float GetTimeFactor() const;

    float GetDeltaTime() const;
    float GetTotalTime() const;

    void Play();
    void Pause();
    void Reset();
    void Update(float dt);
};

inline bool
TimeSource::IsPaused() const
{
    return (pauseCounter > 0);
}

inline void
TimeSource::SetTimeFactor(float _timeFactor)
{
    timeFactor = _timeFactor;
}

inline float
TimeSource::GetTimeFactor() const
{
    return timeFactor;
}

inline float
TimeSource::GetDeltaTime() const
{
    return deltaTime;
}

inline float
TimeSource::GetTotalTime() const
{
    return totalTime;
}

} // namespace Framework
