#include <cstdio>
#include "Core/IO/FileServer.h"
#include "Core/Collections/Dictionary.h"
#include "Core/IO/BitStream.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Log.h"

#ifdef EMSCRIPTEN
#   include <emscripten.h>
#endif

namespace Framework {

DefineClassInfo(Framework::FileServer, Framework::RefCounted);

FileServer::FileServer()
: aliases(Memory::GetAllocator<MallocAllocator>())
{
#if defined EMSCRIPTEN
    this->AddAlias("home", "."); // ToDo
#else
    this->AddAlias("home", "../.."); // ToDo
#endif
    this->AddAlias("shaders_include", "home:"); // ToDo
}

FileServer::~FileServer()
{ }

void
FileServer::AddAlias(const String &alias, const String &path)
{
    String aliasPath = this->ResolvePath(path);

    // add last '\' if not found
    if (aliasPath[aliasPath.Length() - 1] != '/')
        aliasPath += '/';

    aliases[alias] = aliasPath;
}

void
FileServer::RemoveAlias(const String &alias)
{
    aliases.Remove(alias);
}

const String&
FileServer::GetAliasPath(const String &alias) const
{
    assert(aliases.Contains(alias));
    return aliases[alias];
}

String
FileServer::ResolvePath(const String &path) const
{
    int32_t index = path.IndexOf(':');

    if (-1 == index)
        return path;

    String alias = path.Substring(0, index),
           output;

    // replace alias with path
    if (!alias.IsEmpty()) {
        output = this->GetAliasPath(alias);
        ++index;
        if (index < path.Length())
            output.Append(path.Substring(index, path.Length() - index));
    } else {
        output = path;
    }
#ifdef WIN32
    // change '/' to '\'
    int offset = 0;
    while ((index = output.IndexOf('/', offset)) != -1) {
        offset = index;
        output[index] = '\\';
    }
#endif
    return output;
}

int
FileServer::ReadOnly(const char *pathToFile, BitStream &stream) const
{
    String str = this->ResolvePath(pathToFile);
    FILE *f = fopen(str.AsCString(), "rb");
    if (f == nullptr)
    {
        Log::Instance()->Write(Log::Warning, "Cound't open file \"%s\"", str.AsCString());
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    stream.Reset();
    stream.Reserve(sz);
    for (size_t i = 0; i < sz; ++i)//while (!feof(f))
        stream << (char)fgetc(f);
    stream.Rewind();

    fclose(f);

    return 0;
}

int
FileServer::WriteOnly(const char *pathToFile, const BitStream &stream) const
{
    String str = this->ResolvePath(pathToFile);
    FILE *f = fopen(str.AsCString(), "wb");
    if (f == nullptr)
    {
        Log::Instance()->Write(Log::Warning, "Cound't write file \"%s\"", str.AsCString());
        return 1;
    }

    size_t sz = fwrite(stream.GetData(), 1, stream.GetSize(), f);
    fclose(f);

    return (sz == stream.GetSize() ? 0 : 1);
}

} // namespace Framework
