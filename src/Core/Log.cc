#include "Core/Log.h"

#ifdef _WIN32
#	define NOMINMAX
#   include <windows.h>
#endif

#ifdef EMSCRIPTEN
#   include <emscripten.h>
#   include <stdlib.h>
#   include <stdio.h>
#endif // EMSCRIPTEN

namespace Framework {

DefineClassInfo(Framework::Log, Framework::RefCounted) // inherited from Singleton

Log::Log()
{
#ifndef EMSCRIPTEN
    appLog.open("log.txt");
#endif
}

Log::~Log ( )
{
#ifndef EMSCRIPTEN
    appLog.close();
#endif
}

void
Log::Write(int msgFlags, const char *msg, ...)
{
#ifndef EMSCRIPTEN
    assert(appLog.is_open());
#endif // EMSCRIPTEN

    va_list args;
    char    buffer[1024];

    va_start(args, msg);
    vsnprintf(buffer, 1024, msg, args);
    va_end(args);

    bool showMsg = (msgFlags & ShowToUser ? true : false);

    switch ((msgFlags & 0x3)) {
        case Info:
#ifndef EMSCRIPTEN
            appLog << "(i) " << buffer << "\n";
#else
            printf("(i) %s\n", buffer);
#endif
            if (showMsg) {
#if defined _WIN32
                MessageBox(0, buffer, "Info", MB_ICONINFORMATION | MB_OK);
#elif defined __APPLE__
#elif defined EMSCRIPTEN
#endif
            }
            break;
        case Warning:
#ifndef EMSCRIPTEN
            appLog << "(w) " << buffer << "\n";
#else
            printf("(w) %s\n", buffer);
#endif
            if (showMsg) {
#if defined _WIN32
                MessageBox(0, buffer, "Warning", MB_ICONWARNING | MB_OK);
#elif defined __APPLE__
#elif defined EMSCRIPTEN
#endif
            }
            break;
        case Error:
#ifndef EMSCRIPTEN
            appLog << "(e) " << buffer << "\n";
#else
            printf("(e) %s\n", buffer);
#endif
            if (showMsg) {
#if defined _WIN32
                MessageBox(0, buffer, "Error", MB_ICONERROR | MB_OK);
#elif defined __APPLE__
#elif defined EMSCRIPTEN
#endif
            }
            break;
    };

#if defined _DEBUG && !defined EMSCRIPTEN
    appLog.flush();
#endif // _DEBUG && !EMSCRIPTEN
}

void
Log::Barf(const char *msg, ...)
{
    va_list args;
    char    buffer[1024];

    va_start(args, msg);
    vsnprintf(buffer, 1024, msg, args);
    va_end(args);

    printf("%s\n", buffer);
}

}; // namespace Framework
