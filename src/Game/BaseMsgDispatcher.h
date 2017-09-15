#pragma once

#include "Core/RefCounted.h"

namespace Framework {

class BaseMsgDispatcher : public RefCounted {
    DeclareClassInfo;
public:
    BaseMsgDispatcher();
    virtual ~BaseMsgDispatcher();
};

} // namespace Framework
