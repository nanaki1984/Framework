#pragma once

#include "Game/MsgDispatcher_type.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/BlocksAllocator.h"
#include "Core/Collections/Hash.h"
#include "Managers/ComponentsManager.h"
#include "Managers/GetManager.h"

namespace Framework {

inline uint32_t
GetMessageHashCode(const StringHash &msg, Component *component)
{
    return msg.hash + 0x9e3779b9 + (uintptr_t(component) << 6) + (uintptr_t(component) >> 2);
}

template <typename RetType, typename... Args>
inline RetType
MsgDelegate<RetType(Args...)>::operator()(Args... arguments) const
{
    return (*func)(component, arguments...);
}

template <typename RetType, typename... Args>
inline uint32_t
MsgDelegate<RetType(Args...)>::GetHashCode() const
{
    return GetMessageHashCode(msg, component);
}

template <typename RetType, typename... Args>
inline bool
MsgDelegate<RetType(Args...)>::IsValidFor(const StringHash &_msg, Component *_component) const
{
    return (component == _component) && (msg == _msg);
}

template <typename RetType, typename... Args>
MsgDelegate<RetType(Args...)>::MsgDelegate()
{ }

template <typename RetType, typename... Args>
MsgDelegate<RetType(Args...)>::~MsgDelegate()
{ }

template <typename RetType, typename... Args>
MsgDispatcher<RetType(Args...)> *MsgDispatcher<RetType(Args...)>::instance = nullptr;

template <typename RetType, typename... Args>
inline void
MsgDispatcher<RetType(Args...)>::Initialize()
{
    assert(instance == nullptr);
    SmartPtr<BaseMsgDispatcher> p = SmartPtr<BaseMsgDispatcher>::MakeNew<BlocksAllocator, MsgDispatcher<RetType(Args...)>>();
    GetManager<ComponentsManager>()->AddMsgDispatcher(p);
}

template <typename RetType, typename... Args>
inline void
MsgDispatcher<RetType(Args...)>::Register(const MsgDelegate<RetType(Args...)> &d)
{
    delegates.Add(d.GetHashCode(), d);
}

template <typename RetType, typename... Args>
inline void
MsgDispatcher<RetType(Args...)>::Remove(const StringHash &msg, Component *component)
{
    MsgDelegate<RetType(Args...)> *d = delegates.Get(GetMessageHashCode(msg, component));
    while (d != nullptr) {
        if (d->IsValidFor(msg, component))
            break;
        d = delegates.Next(d);
    }

    if (d != nullptr)
        delegates.Remove(d);
}

template <typename RetType, typename... Args>
inline RetType
MsgDispatcher<RetType(Args...)>::Dispatch(const StringHash &msg, Component *component, Args... arguments)
{
    MsgDelegate<RetType(Args...)> *d = delegates.Get(GetMessageHashCode(msg, component));
    while (d != nullptr) {
        if (d->IsValidFor(msg, component))
            break;
        d = delegates.Next(d);
    }

    if (nullptr == d)
        return RetType();
    else
        return (*d)(arguments...);
}

template <typename RetType, typename... Args>
inline MsgDispatcher<RetType(Args...)>*
MsgDispatcher<RetType(Args...)>::Instance()
{
    if (nullptr == instance)
        Initialize();
    return instance;
}

template <typename RetType, typename... Args>
MsgDispatcher<RetType(Args...)>::MsgDispatcher()
: delegates(Memory::GetAllocator<MallocAllocator>())
{
    assert(instance == nullptr);
    instance = this;
}

template <typename RetType, typename... Args>
MsgDispatcher<RetType(Args...)>::~MsgDispatcher()
{
    assert(instance == this);
    instance = nullptr;
}

template <typename RetType, typename... Args>
template <typename T, RetType (T::*Mem_fun_addr)(Args...)>
inline MsgDelegate<RetType(Args...)>
MsgDelegate<RetType(Args...)>::Create(const StringHash &msg, T *obj)
{
    MsgDelegate<RetType(Args...)> d;
    d.msg = msg;
    d.component = static_cast<Component*>(obj);
    d.func = [](void* obj, Args... args){ return (static_cast<T*>(obj)->*Mem_fun_addr)(args...); };
    MsgDispatcher<RetType(Args...)>::Instance()->Register(d);
    return d;
}

} // namespace Game
