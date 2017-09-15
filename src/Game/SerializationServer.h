#pragma once

#include "Core/RefCounted.h"
#include "Core/Collections/Hash_type.h"
#include "Core/Pool/HandleList_type.h"
#include "Game/Entity.h"

namespace Framework {

class SerializationServer : public RefCounted {
    DeclareClassInfo;
protected:
    Hash<Handle<BaseObject>> loadedObjects;

//#if defined(EDITOR)
    uint32_t nextUniqueSerializationId;
    HandleList<Entity, &Entity::serializationNode> entitiesToSerialize;
//#endif
public:
    SerializationServer();
    virtual ~SerializationServer();

    void DeserializeEntities(const BitStream &stream);
    void AddLoadingObject(uint32_t id, BaseObject *object);
    Handle<BaseObject> GetLoadingObject(uint32_t id) const;
    
//#if defined(EDITOR)
    void MarkEntityForSerialization(const Handle<Entity> &entity);
    void SerializeMarkedEntities(BitStream &stream);
    void CompleteSerialization();
//#endif
};

} // namespace Framework
