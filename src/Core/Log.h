#pragma once

#include <cstdarg>
#include <fstream>

#include "Core/Singleton.h"

namespace Framework {

class Log : public Singleton<Log> {
    DeclareClassInfo;
protected:
    std::ofstream appLog;
public:
    enum MsgFlags {
        Info    = 0x0,   // 000
        Warning = 0x1,   // 001
        Error   = 0x2,   // 010

        ShowToUser = 0x4 // 100
    };

    Log();
    virtual ~Log();

    void Write(int msgFlags, const char *msg, ...);

    static void Barf(const char *msg, ...);
};

}; // namespace Framework
