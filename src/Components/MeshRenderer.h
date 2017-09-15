#pragma once

#include "Components/Renderer.h"

namespace Framework {

class MeshRenderer : public Renderer {
    DeclareClassInfo;
	DeclareComponent;
protected:
    void UpdateBounds();
public:
    MeshRenderer();
    MeshRenderer(const MeshRenderer &other);
    MeshRenderer(MeshRenderer &&other);
    virtual ~MeshRenderer();

    const WeakPtr<Mesh>& GetMesh() const;
    void SetMesh(const WeakPtr<Mesh> &newMesh);
    void SetMesh(const char *filename, Resource::Access access = Resource::ReadOnly);

    void Deserialize(SerializationServer *server, const BitStream &stream);
    void Render();

//#if defined(EDITOR)
    void OnSerialize(BitStream &stream);
//#endif
};

} // namespace Framework
