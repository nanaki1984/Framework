#pragma once

#include "Math/Bounds.h"
#include "Render/Resources/Mesh.h"
#include "Render/Resources/Material.h"
#include "Game/Component.h"

namespace Framework {

class RenderersManager;

class Renderer : public Component {
    DeclareClassInfo;
	DeclareComponent;
protected:
    Math::Bounds bounds;
    WeakPtr<Mesh> mesh;
    Array<WeakPtr<Material>> materials;

	ListNode<Renderer> octreeNode;

    uint8_t sortingOrder;
public:
    Renderer();
	Renderer(const Renderer &other);
	Renderer(Renderer &&other);
	virtual ~Renderer();
	
	const Math::Bounds& GetBounds() const;

    uint32_t GetMaterialsCount() const;

    const WeakPtr<Material>& GetMaterial(uint32_t index = 0) const;
    void SetMaterial(const WeakPtr<Material> &material, uint32_t index = 0);
    void SetMaterial(const char *filename, Resource::Access access = Resource::ReadOnly, uint32_t index = 0);

    uint8_t GetSortingOrder() const;
    void SetSortingOrder(uint8_t value);

    void OnCreate();

    friend class RenderersManager;
};

} // namespace Framework
