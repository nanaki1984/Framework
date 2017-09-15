#pragma once

#include "Core/Application.h"

namespace Framework {

template <typename T>
T* GetManager()
{
    return static_cast<T*>(Application::Instance()->GetManager(&T::RTTI).Get());
}

} // namespace Framework
