#include "Core/IO/Path.h"
#include "Core/String.h"

namespace Framework {
    namespace Path {
            
String
GetDirectoryName(const String &path)
{
    int32_t index1 = path.LastIndexOf(':'),
            index2 = path.LastIndexOf('\\');

    return path.Substring(0, (index1 > index2 ? index1 : index2) + 1);
}

String
GetFileName(const String &path)
{
    int32_t index1 = path.LastIndexOf(':'),
            index2 = path.LastIndexOf('\\'),
            index3 = path.LastIndexOf('.');

    if (-1 == index1) index1 = 0;
    if (-1 == index2) index2 = 0;

    int32_t start = (index1 > index2 ? index1 : index2) + 1,
            size  = (-1 == index3 ? path.Length() - start : index3 - start);

    return path.Substring(start, size);
}

String
GetFileExtension(const String &path)
{
    int32_t index = path.LastIndexOf('.');
    if (-1 == index)
        return String(path.GetAllocator());
    else {
        ++index;
        return path.Substring(index, path.Length() - index);
    }
}
            
    } // namespace Path
} // namespace Framework
