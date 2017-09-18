#pragma once

#include "Render/KeyCode.h"
#include "Render/Base/BaseShaderProgram.h"
#include "Render/Base/BaseRenderer.h"

namespace Framework {
	namespace RHI {

class Key {
public:
    static const uint8_t kCommandDrawCall        = 0x00;
    static const uint8_t kCommandSetRenderTarget = 0x01;
    static const uint8_t kCommandSetViewport     = 0x02;
    static const uint8_t kCommandClear           = 0x03;
    static const uint8_t kCommandDispatch        = 0x04;

    static const uint32_t kResetRenderTarget = 0xFFFFFFFF;

    uint8_t cameraDepth;
    uint8_t cameraLayer;
private:
    bool isCommand;

    union {
        struct { // COMMANDS
            uint8_t sequenceNumber;
            uint8_t commandId;

            union {
                struct { // kCommandSetRenderTarget
                    CubeFace rtCubeface;
                    uint32_t rtId;
                };

                Viewport viewport; // kCommandSetViewport

                struct { // kCommandClear
                    BaseRenderer::ClearFlags clearFlags;
                    uint32_t				 clearColor;
                    float					 clearDepth;
                    uint32_t				 clearStencil;
                };

                struct { // kCommandDispatch
                    uint32_t computeShaderId;
                    uint16_t numGroupsX;
                    uint16_t numGroupsY;
                    uint16_t numGroupsZ;
                    uint16_t dispatchParamsBlockId;
                };
            };
        };

        struct { // kCommandDrawCall
            BaseShaderProgram::Translucency translucency;

            uint8_t sortingOrder;

            uint32_t materialId;
            uint8_t  materialPass;

            uint32_t meshId;
            uint8_t  subMeshIndex;

            uint16_t paramsBlockId;

            float depth;
        };
    };
public:
    Key();
    Key(const KeyCode &code);

    operator KeyCode() const;

    uint8_t GetCommand() const;

    // kCommandSetRenderTarget
    CubeFace GetRenderTargetCubeface() const;
    uint32_t GetRenderTargetId() const;

    // kCommandSetViewport
    const Viewport& GetViewport() const;

    // kCommandClear
    BaseRenderer::ClearFlags GetClearFlags() const;
    uint32_t GetClearColor() const;
    float GetClearDepth() const;
    uint32_t GetClearStencil() const;

    // kCommandDispatch
    uint32_t GetComputeShaderId() const;
    void GetNumWorkGroups(int &x, int &y, int &z) const;
    uint16_t GetDispatchParamsBlockId() const;

    // DrawCall
    uint32_t GetMaterialId() const;
    uint8_t GetMaterialPass() const;
    uint32_t GetMeshId() const;
    uint8_t GetSubMeshIndex() const;
    uint16_t GetParamsBlockId() const;
    float GetDepth() const;

    Key& Sequence(uint8_t sequenceNumber);
    Key& ResetRenderTarget();
    Key& SetRenderTarget(const SmartPtr<RenderTarget> &rt,
                         CubeFace cubeFace = CubeFace::PositiveX);
    Key& SetViewport(const Viewport &_viewport);
    Key& Clear(BaseRenderer::ClearFlags _clearFlags,
               uint32_t _clearColor,
               float _clearDepth,
               uint32_t _clearStencil);
    Key& Dispatch(const WeakPtr<ComputeShader> &computeShader,
                  int _numGroupsX, int _numGroupsY, int _numGroupsZ,
                  uint16_t _paramsBlockId);
    Key& DrawCall(uint8_t _sortingOrder,
                  const WeakPtr<Material> &material,
                  uint8_t _materialPass,
                  const WeakPtr<Mesh> &mesh,
                  uint8_t _subMeshIndex,
                  uint16_t _paramsBlockId,
                  float _depth);
};

inline uint8_t
Key::GetCommand() const
{
    if (isCommand)
        return commandId;
    else
        return kCommandDrawCall;
}

inline CubeFace
Key::GetRenderTargetCubeface() const
{
    return rtCubeface;
}

inline uint32_t
Key::GetRenderTargetId() const
{
    return rtId;
}

inline const Viewport&
Key::GetViewport() const
{
    return viewport;
}

inline BaseRenderer::ClearFlags
Key::GetClearFlags() const
{
    return clearFlags;
}

inline uint32_t
Key::GetClearColor() const
{
    return clearColor;
}

inline float
Key::GetClearDepth() const
{
    return clearDepth;
}

inline uint32_t
Key::GetClearStencil() const
{
    return clearStencil;
}

inline uint32_t
Key::GetComputeShaderId() const
{
    return computeShaderId;
}

inline void
Key::GetNumWorkGroups(int &x, int &y, int &z) const
{
    x = numGroupsX;
    y = numGroupsY;
    z = numGroupsZ;
}

inline uint16_t
Key::GetDispatchParamsBlockId() const
{
    return dispatchParamsBlockId;
}

inline uint32_t
Key::GetMaterialId() const
{
    return materialId;
}

inline uint8_t
Key::GetMaterialPass() const
{
    return materialPass;
}

inline uint32_t
Key::GetMeshId() const
{
    return meshId;
}

inline uint8_t
Key::GetSubMeshIndex() const
{
    return subMeshIndex;
}

inline uint16_t
Key::GetParamsBlockId() const
{
    return paramsBlockId;
}

inline float
Key::GetDepth() const
{
    return depth;
}

	} // namespace RHI
} // namespace Framework
