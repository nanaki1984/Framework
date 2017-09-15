#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"
#include "Core/Collections/Dictionary_type.h"

namespace Framework {

class BitStream;

class FileServer : public Singleton<FileServer> {
    DeclareClassInfo;
private:
    Dictionary<String, String> aliases;
public:
    FileServer();
    FileServer(const FileServer &other) = delete;
    virtual ~FileServer();

    FileServer& operator =(const FileServer &other) = delete;

    void AddAlias(const String &alias, const String &path);
    void RemoveAlias(const String &alias);
    const String& GetAliasPath(const String &alias) const;

    String ResolvePath(const String &path) const;

    int ReadOnly(const char *pathToFile, BitStream &stream) const;
    int WriteOnly(const char *pathToFile, const BitStream &stream) const;

    //BitStream Map(const char *pathToFile, uint32_t offset, uint32_t size) const;
    //void Unmap(const BitStream &stream);

    //void Download(const char *pathToFile, void (*func)(const BitStream&)) const;
};

} // namespace Framework
