#pragma once

#include "Core/Collections/Array_type.h"

namespace Framework {
	namespace RHI {

class VertexDecl {
public:
    enum VertexType {
        XYZ,            // position
        Normal,         // normal
        Tangent,        // tangent
        Binormal,       // binormal
        BlendWeights,   // blending weights
        BlendIndices,   // blending matrix indices
        TexCoord,       // texture coords
        Color0,         // first color
        Color1          // second color
    };

    enum VertexSize {
        Float1 =  4,
        Float2 =  8,
        Float3 = 12,
        Float4 = 16
    };

    struct VertexElement {
        VertexType type;
        VertexSize size;
        uint32_t   offset;
    };
private:
    Array<VertexElement> elements;
public:
    VertexDecl();
    VertexDecl(const VertexDecl &other);
    VertexDecl(VertexDecl &&other);
    ~VertexDecl();

    VertexDecl& operator =(const VertexDecl &other);
    VertexDecl& operator =(VertexDecl &&other);

    uint32_t AddVertexElement(VertexType type, VertexSize size);

    uint32_t GetOffset(VertexType type, VertexSize &size, uint32_t wantedSize = 0, uint32_t n = 1) const;

    const VertexElement& GetElement(uint32_t index) const;
    uint32_t GetElementsCount() const;
    uint32_t GetVertexStride() const;
};

	} // namespace RHI
} // namespace Framework
