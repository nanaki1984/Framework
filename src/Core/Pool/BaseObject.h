#pragma once

#include <cstdint>
#include "Core/ClassInfo.h"

namespace Framework {

class BasePool;

class BaseObject {
    DeclareRootClassInfo;
protected:
    BasePool *pool;
    uint32_t id;
public:
    BaseObject();
    BaseObject(const BaseObject &other);
    BaseObject(BaseObject &&other);
    virtual ~BaseObject();

    BasePool* GetPool() const;
    uint32_t GetInstanceID() const;

    void Destroy();

    friend class BasePool;
};

} // namespace Framework
