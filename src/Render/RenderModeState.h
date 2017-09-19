#pragma once

#include <cstdint>
#include "Core/EnumStrings.h"
#include "Render/Utils/Rect.h"

namespace Framework {

struct RenderModeState {
    // Cull mode
    enum CullMode {
        None = 0,
        Front,
        Back
    } cullMode;

    // Color mask
    enum ColorMask {
        NoColor = 0,

        Alpha = (1 << 0),
        Red   = (1 << 1),
        Green = (1 << 2),
        Blue  = (1 << 3)
    } colorMask;

    // Depth buffer
    bool zEnable;
    bool zWriteEnable;

    enum CompareFunc {
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    } zFunc;

    // Blending
    bool blendEnable;

    enum BlendOp {
        Add = 0,
        Sub,
        RevSub,
        Min,
        Max
    } blendOp;

    enum BlendFactor {
        Zero = 0,
        One,
        SrcColor,
        InvSrcColor,
        SrcAlpha,
        InvSrcAlpha,
        DstAlpha,
        InvDstAlpha,
        DstColor,
        InvDstColor,
        SrcAlphaSat
    } blendSrcFactor, blendDstFactor;

    // Alpha test
    bool        alphaEnable;
    CompareFunc alphaFunc;
    uint32_t    alphaRef;

    // Stencil
    enum StencilMode {
        Inactive = 0,
        Normal,
        TwoSided,
    } stencilMode;
    CompareFunc stencilFunc, stencilFuncCCW;
    uint32_t    stencilRef, stencilMask;
    enum StencilOp {
        Keep = 0,
        Null,
        Replace,
        IncSat,
        DecSat,
        Invert,
        Inc,
        Dec,
    } stencilFail, stencilZFail, stencilPass,
      stencilFailCCW, stencilZFailCCW, stencilPassCCW;

    bool scissorTestEnabled;
    int scissorTestRect[4];

    // ToDo: clip planes

    RenderModeState();

    bool operator ==(const RenderModeState &state) const;
    bool operator !=(const RenderModeState &state) const;
};

} // namespace Framework

DeclareEnumStrings(Framework::RenderModeState::CullMode, 3);
DeclareEnumStrings(Framework::RenderModeState::ColorMask, 16);
DeclareEnumStrings(Framework::RenderModeState::CompareFunc, 8);
DeclareEnumStrings(Framework::RenderModeState::BlendOp, 5);
DeclareEnumStrings(Framework::RenderModeState::BlendFactor, 11);
DeclareEnumStrings(Framework::RenderModeState::StencilMode, 3);
DeclareEnumStrings(Framework::RenderModeState::StencilOp, 8);
