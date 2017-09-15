#pragma once

#include "Game/Component.h"
#include "Math/Matrix.h"
#include "Render/Base/BaseRenderer.h"
#include "Render/Utils/Rect.h"
#include "Render/Utils/Color.h"

namespace Framework {

class Camera : public Component {
	DeclareClassInfo;
	DeclareComponent;
protected:
	Rect pixelRect;

	float nearClipPlane;
	float farClipPlane;
	float fieldOfView;
	float aspect;

    mutable Math::Matrix projection;
	bool projectionOverride;
	mutable bool projectionDirty;

    RHI::BaseRenderer::ClearFlags clearFlags;
    Color clearColor;

    uint8_t depth;
public:
	Camera();
	Camera(const Camera &other);
	Camera(Camera &&other);
	virtual ~Camera();

    void Deserialize(SerializationServer *server, const BitStream &stream);
    void Render();
    
    const Rect& GetPixelRect() const;

	float GetNearClipPlane() const;
	float GetFarClipPlane() const;
	float GetFieldOfView() const;
	float GetAspect() const;

    RHI::BaseRenderer::ClearFlags GetClearFlags() const;
    const Color& GetClearColor() const;

    uint8_t GetDepth() const;

	const Math::Matrix& GetProjection() const;

	void SetPixelRect(const Rect &rect);

	void SetNearClipPlane(float distance);
	void SetFarClipPlane(float distance);
	void SetFieldOfView(float angle);

	void SetProjection(const Math::Matrix &matrix);
	void ResetProjection();

    void SetClearFlags(RHI::BaseRenderer::ClearFlags flags);
    void SetClearColor(const Color &color);

    void SetDepth(uint8_t cameraDepth);

//#if defined(EDITOR)
    void OnSerialize(BitStream &stream);
//#endif
};

} // namespace Framework
