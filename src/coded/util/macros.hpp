#pragma once

#if defined(__unix__)
#define IKA_OS_UNIX
#endif

#if defined(linux)
#define IKA_OS_LINUX
#elif defined(_WIN32) || defined(WIN32)
#define IKA_OS_WIN
#endif
