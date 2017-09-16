#pragma once

namespace Framework {

void ShowAssertMessage(const char *exp, const char *filename, int line);
void ShowAssert2Message(const char *exp, const char *msg, const char *filename, int line);

} // namespace Framework

#ifdef NDEBUG // Xcode
#   undef _DEBUG
#endif

#ifdef assert
#   undef assert
#endif // assert

#ifdef _DEBUG
#   if defined(_MSC_VER)
#       include <intrin.h>
#       define _BREAK __debugbreak();
#   else
#       define _BREAK __asm int 03
#   endif
#   define assert(_Exp)       {if(!(_Exp)){Framework::ShowAssertMessage(#_Exp, __FILE__, __LINE__); _BREAK}}
#   define assert2(_Exp,_Msg) {if(!(_Exp)){Framework::ShowAssert2Message(#_Exp, _Msg, __FILE__, __LINE__); _BREAK}}
#   define verify(_Exp)       {if(!(_Exp)){Framework::ShowAssertMessage(#_Exp, __FILE__, __LINE__); _BREAK}}
#   define verify2(_Exp,_Msg) {if(!(_Exp)){Framework::ShowAssert2Message(#_Exp, _Msg, __FILE__, __LINE__); _BREAK}}
#else
#   define assert(_Exp)       ((void)0)
#   define assert2(_Exp,_Msg) ((void)0)
#   define verify(_Exp)       ((void)(_Exp))
#   define verify2(_Exp,_Msg) ((void)(_Exp))
#endif
