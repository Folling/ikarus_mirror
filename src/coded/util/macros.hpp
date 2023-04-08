#pragma once

#if defined(__unix__)
#define IKA_OS_FAMILY_UNIX
#endif

#define IKA_API
#if defined(linux)
#define IKA_OS_LINUX
#define IKA_API __attribute__((visibility("default")))
#elif defined(_WIN32) || defined(WIN32)
#define IKA_OS_WIN
#define IKA_API __declspec(dllexport)
#endif
