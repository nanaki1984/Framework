#pragma once

#include "Core/RefCounted.h"

namespace Framework {

class BaseManager : public RefCounted {
    DeclareClassInfo;
public:
    BaseManager();
    virtual ~BaseManager();

    virtual int GetExecutionOrder() = 0;

    virtual void OnUpdate() = 0;
    virtual void OnLateUpdate() = 0;
	virtual void OnRender() = 0;
    virtual void OnPause() = 0;
    virtual void OnResume() = 0;
    virtual void OnQuit() = 0;
};

#define DeclareManager \
public: \
    virtual int GetExecutionOrder();

#define DefineManager(type,execOrder) \
    int type::GetExecutionOrder() { return execOrder; };

} // namespace Framework
