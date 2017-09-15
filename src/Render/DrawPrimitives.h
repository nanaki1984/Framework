#pragma once

#include <cstdint>

namespace Framework {

struct DrawPrimitives {
    enum PrimitiveType {
        InvalidPrimitiveType = 0,

        PointList     = 1,
        LineList      = 2,
        LineStrip     = 3,
        TriangleList  = 4,
        TriangleStrip = 5,
        TriangleFan   = 6
    };

    PrimitiveType primType;    // primitive type
    uint32_t      startIndex;  // start index in IB
    //uint32_t      endIndex;    // end index in IB
    uint32_t      nPrimitives; // primitive count
    //uint32_t      nVertices;   // # of vertices used
};

} // namespace Framework
