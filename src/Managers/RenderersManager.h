#pragma once

#include "Managers/BaseManager.h"
#include "Core/Collections/List_type.h"
#include "Components/Renderer.h"
#include "Math/Plane.h"
#include "Math/Vector3.h"
#include "Math/Matrix.h"
#include "Math/Bounds.h"

namespace Framework {

class RenderersManager : public BaseManager {
	DeclareClassInfo;
    DeclareManager;
protected:
	const int kOctreeMaxDepth = 8;
    const float kOctreeMinRadius = 4.0f;

	class OctreeNode {
	private:
		OctreeNode(const OctreeNode &node);
		OctreeNode& operator =(const OctreeNode &node);
	public:
		OctreeNode(const Math::Vector3 &_center, float _radius);
		OctreeNode(OctreeNode &&node);

		Math::Vector3 center;
		float radius;
		List<Renderer, &Renderer::octreeNode> renderers;
		OctreeNode *children[8];
	};

	class Frustum {
	private:
		Math::Plane planes[6];
		Math::Vector3 corners[8];
	public:
		Frustum(const Math::Matrix &viewProjection);

		bool Intersects(const Math::Bounds &bounds) const;
	};

	OctreeNode* RequestNode(int depth, const Math::Vector3 &center);
	void FreeNodesRecursively(OctreeNode *node);
    void QueryRenderersRecursively(const Frustum &frustum, OctreeNode *node, Array<Handle<Renderer>> &out);

	OctreeNode *octreeRoot;
    Array<Handle<Renderer>> lastQueryResult;
public:
	RenderersManager();
	virtual ~RenderersManager();

	virtual void OnUpdate();
	virtual void OnLateUpdate();
	virtual void OnRender();
	virtual void OnPause();
	virtual void OnResume();
	virtual void OnQuit();

    uint32_t QueryRenderers(const Math::Matrix &viewProjection);
    const Handle<Renderer>& GetRenderer(uint32_t index);
};

} // namespace Framework
