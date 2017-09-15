#include "Game/SerializationServer.h"
#include "Managers/EntitiesManager.h"
#include "Managers/ComponentsManager.h"
#include "Managers/GetManager.h"

namespace Framework {

DefineClassInfo(Framework::SerializationServer, Framework::RefCounted);

SerializationServer::SerializationServer()
: loadedObjects(Memory::GetAllocator<MallocAllocator>())
//#if defined(EDITOR)
, nextUniqueSerializationId(0)
//#endif
{ }

SerializationServer::~SerializationServer()
{
    entitiesToSerialize.Clear();
}

//#if defined(EDITOR)
void
SerializationServer::MarkEntityForSerialization(const Handle<Entity> &entity)
{
    entitiesToSerialize.PushBack(entity);
    entity->MarkForSerialization(nextUniqueSerializationId);
}

void
SerializationServer::SerializeMarkedEntities(BitStream &stream)
{
    Log::Instance()->Write(Log::Info, "Saving %d entities...", entitiesToSerialize.Count());

    stream << entitiesToSerialize.Count();

    auto it = entitiesToSerialize.Begin();
    while (it != nullptr) {
        it->Serialize(stream);

        it = entitiesToSerialize.GetNext(it);
    }

    nextUniqueSerializationId = 0;
    entitiesToSerialize.Clear();
}

void
SerializationServer::CompleteSerialization()
{
    auto it = entitiesToSerialize.Begin();
    while (it != nullptr) {
        it->ClearSerializationIds();

        it = entitiesToSerialize.GetNext(it);
    }

    nextUniqueSerializationId = 0;
    entitiesToSerialize.Clear();
}
//#endif

void
SerializationServer::DeserializeEntities(const BitStream &stream)
{
    uint32_t numEntities;
    stream >> numEntities;

    Log::Instance()->Write(Log::Info, "Loading %d entities...", numEntities);

    auto entMng = GetManager<EntitiesManager>();
    for (uint32_t i = 0; i < numEntities; ++i)
        entMng->NewVoidEntity()->Deserialize(this, stream);

    GetManager<ComponentsManager>()->DeserializeComponents(this, stream);

    loadedObjects.Clear();
}

void
SerializationServer::AddLoadingObject(uint32_t id, BaseObject *object)
{
    loadedObjects.Add(id, object);
}

Handle<BaseObject>
SerializationServer::GetLoadingObject(uint32_t id) const
{
    auto it = loadedObjects.Get(id);
    if (nullptr == it)
        return Handle<BaseObject>::Null;
    else
        return *it;
}

} // namespace Game
