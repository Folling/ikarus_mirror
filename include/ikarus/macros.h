#pragma once

#if defined(__unix__)

#define IKA_OS_FAMILY_UNIX
#define IKA_API __attribute__((visibility("default")))

#if defined(linux)
#define IKA_OS_LINUX
#endif

#elif defined(_WIN32) || defined(WIN32)
#define IKA_OS_WIN
#define IKA_API __declspec(dllexport)
#endif

#ifndef IKA_API
#define IKA_API
#endif
