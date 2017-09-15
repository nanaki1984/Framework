#include "Render/RenderModeState.h"

namespace Framework {

RenderModeState::RenderModeState()
// setting default mode
: cullMode(Back),
  colorMask(static_cast<ColorMask>(Alpha | Red | Green | Blue)),
  zEnable(true),
  zWriteEnable(true),
  zFunc(LessEqual),
  blendEnable(false),
  blendOp(Add),
  blendSrcFactor(One),
  blendDstFactor(Zero),
  alphaEnable(false),
  alphaFunc(Greater),
  alphaRef(0),
  stencilMode(Inactive),
  stencilFunc(Always),
  stencilFuncCCW(Always),
  stencilRef(0),
  stencilMask(0),
  stencilFail(Keep),
  stencilZFail(Keep),
  stencilPass(Keep),
  stencilFailCCW(Keep),
  stencilZFailCCW(Keep),
  stencilPassCCW(Keep)
  // ToDo: clip planes
{ }

bool
RenderModeState::operator ==(const RenderModeState &state) const
{
    return ((cullMode == state.cullMode) &&
            (colorMask == state.colorMask) &&
            (zEnable == state.zEnable) &&
            (zWriteEnable == state.zWriteEnable) &&
            (zFunc == state.zFunc) &&
            (blendEnable == state.blendEnable) &&
            (blendOp == state.blendOp) &&
            (blendSrcFactor == state.blendSrcFactor) &&
            (blendDstFactor == state.blendDstFactor) &&
            (alphaEnable == state.alphaEnable) &&
            (alphaFunc == state.alphaFunc) &&
            (alphaRef == state.alphaRef) &&
            (stencilMode == state.stencilMode) &&
            (stencilFunc == state.stencilFunc) &&
            (stencilFuncCCW == state.stencilFuncCCW) &&
            (stencilRef == state.stencilRef) &&
            (stencilMask == state.stencilMask) &&
            (stencilFail == state.stencilFail) &&
            (stencilZFail == state.stencilZFail) &&
            (stencilPass == state.stencilPass) &&
            (stencilFailCCW == state.stencilFailCCW) &&
            (stencilZFailCCW == state.stencilZFailCCW) &&
            (stencilPassCCW == state.stencilPassCCW));
            // ToDo: clip planes
}

bool
RenderModeState::operator !=(const RenderModeState &state) const
{
    return !(this->operator==(state));
}

} // namespace Framework

DefineEnumStrings(Framework::RenderModeState::CullMode) = {
    "None",
    "Front",
    "Back"
};

DefineEnumStrings(Framework::RenderModeState::ColorMask) = {
    "NoColor",          // 0
    "Alpha",            // 1
    "Red",              // 2
    "AlphaRed",         // 3
    "Green",            // 4
    "AlphaGreen",       // 5
    "RedGreen",         // 6
    "AlphaRedGreen",    // 7
    "Blue",             // 8
    "AlphaBlue",        // 9
    "RedBlue",          //10
    "AlphaRedBlue",     //11
    "GreenBlue",        //12
    "AlphaGreenBlue",   //13
    "RedGreenBlue",     //14
    "AlphaRedGreenBlue" //15
};

DefineEnumStrings(Framework::RenderModeState::CompareFunc) = {
    "Never",
    "Less",
    "Equal",
    "LessEqual",
    "Greater",
    "NotEqual",
    "GreaterEqual",
    "Always"
};

DefineEnumStrings(Framework::RenderModeState::BlendOp) = {
    "Add",
    "Sub",
    "RevSub",
    "Min",
    "Max"
};

DefineEnumStrings(Framework::RenderModeState::BlendFactor) = {
    "Zero",
    "One",
    "SrcColor",
    "InvSrcColor",
    "SrcAlpha",
    "InvSrcAlpha",
    "DstAlpha",
    "InvDstAlpha",
    "DstColor",
    "InvDstColor",
    "SrcAlphaSat"
};

DefineEnumStrings(Framework::RenderModeState::StencilMode) = {
    "Inactive",
    "Normal",
    "TwoSided"
};

DefineEnumStrings(Framework::RenderModeState::StencilOp) = {
    "Keep",
    "Null",
    "Replace",
    "IncSat",
    "DecSat",
    "Invert",
    "Inc",
    "Dec"
};
