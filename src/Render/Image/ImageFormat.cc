#include "Render/Image/ImageFormat.h"

namespace Framework {

ImageFormat::DataStruct ImageFormat::DataFormats[] = {
    { { { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(0),                                        0,  0 },
    { { { 0, 8}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasLuminance),               1,  0 },
    { { { 8, 8}, { 0, 8}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasAlpha | HasLuminance),    2,  0 },
    { { { 0, 0}, { 0, 5}, { 5, 6}, {11, 5} }, ImageFormat::Flags(IntegerType),                              2,  0 },
    { { {15, 1}, { 0, 5}, { 5, 5}, {10, 5} }, ImageFormat::Flags(IntegerType | HasAlpha),                   2,  0 },
    { { {12, 4}, { 0, 4}, { 4, 4}, { 8, 4} }, ImageFormat::Flags(IntegerType | HasAlpha),                   2,  0 },
    { { { 0,16}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasLuminance),               2,  0 },
    { { { 0,16}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(HalfFloatType | HasLuminance),             2,  0 },
    { { {30, 2}, { 0,10}, {10,10}, {20,10} }, ImageFormat::Flags(IntegerType | HasAlpha),                   4,  0 },
    { { {16,16}, { 0,16}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasAlpha | HasLuminance),    4,  0 },
    { { { 0, 0}, { 0, 8}, { 8, 8}, {16, 8} }, ImageFormat::Flags(IntegerType),                              4,  0 },
    { { {24, 8}, { 0, 8}, { 8, 8}, {16, 8} }, ImageFormat::Flags(IntegerType | HasAlpha),                   4,  0 },
    { { {16,16}, { 0,16}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(HalfFloatType | HasAlpha | HasLuminance),  4,  0 },
    { { { 0,32}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(FloatType | HasLuminance),                 4,  0 },
    { { {32,32}, { 0,32}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(FloatType | HasAlpha | HasLuminance),      8,  0 },
    { { {48,16}, { 0,16}, {16,16}, {32,16} }, ImageFormat::Flags(HalfFloatType | HasAlpha),                 8,  0 },
    { { {96,32}, { 0,32}, {32,32}, {64,32} }, ImageFormat::Flags(FloatType | HasAlpha),                    16,  0 },
    { { { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | Compressed),                 0,  8 },
    { { { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasAlpha | Compressed),      0, 16 },
    { { { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }, ImageFormat::Flags(IntegerType | HasAlpha | Compressed),      0, 16 }
};

ImageFormat::ImageFormat(Type t)
: type(t),
  data(DataFormats + t)
{ }

} // namespace Framework
