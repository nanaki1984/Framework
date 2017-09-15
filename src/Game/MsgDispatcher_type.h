#pragma once

#include <functional>
#include "Game/BaseMsgDispatcher.h"
#include "Core/Collections/Hash_type.h"
#include "Core/StringHash.h"

namespace Framework {

class Component;

uint32_t GetMessageHashCode(const StringHash &msg, Component *component);

template <typename... Args> class MsgDelegate { };

template <typename RetType, typename... Args>
class MsgDelegate<RetType(Args...)>
{
public:
    template <typename T, RetType(T::*Mem_fun_addr)(Args...)>
    static MsgDelegate<RetType(Args...)> Create(const StringHash &msg, T *obj);

    RetType operator()(Args... arguments) const;

    uint32_t GetHashCode() const;

    bool IsValidFor(const StringHash &_msg, Component *_component) const;

    MsgDelegate();
    ~MsgDelegate();
private:
    StringHash msg;
    Component *component;
    RetType(*func)(void *, Args...);
};

template <typename... Args> class MsgDispatcher { };

template <typename RetType, typename... Args>
class MsgDispatcher<RetType(Args...)> : public BaseMsgDispatcher
{
private:
    Hash<MsgDelegate<RetType(Args...)>> delegates;

    static MsgDispatcher<RetType(Args...)> *instance;

    static void Initialize();
public:
    void Register(const MsgDelegate<RetType(Args...)> &d);

    void Remove(const StringHash &msg, Component *component);

    RetType Dispatch(const StringHash &msg, Component *component, Args... arguments);

    static MsgDispatcher<RetType(Args...)>* Instance();

    MsgDispatcher();
    virtual ~MsgDispatcher();
};

} // namespace Game
