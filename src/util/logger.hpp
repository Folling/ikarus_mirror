#pragma once

#include <iostream>
#include <string_view>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <util/format.hpp>

#define GEN_LOG_LEVEL_FUNC(name, _level, out, colour, log_name)                                                        \
    template<typename... Ts>                                                                                           \
    void name##_impl(int line, char const * file, char const * function, fmt::format_string<Ts...> msg, Ts&&... ts) {  \
        if (threshold > _level) [[likely]] {                                                                           \
            return;                                                                                                    \
        }                                                                                                              \
                                                                                                                       \
        fmt::print(out, fmt::fg(fmt::color::colour), log_name "({}@{}:{}) ", shorten_file_name(file), function, line); \
        fmt::print(out, fmt::fg(fmt::color::light_gray), fmt::format(msg, std::forward<Ts>(ts)...));                   \
        fmt::print(out, "\n");                                                                                         \
    }

enum class LogLevel { Debug = 0, Trace = 1, Verbose = 2, Info = 3, Warning = 4, Error = 5, Fatal = 6 };

inline LogLevel threshold{LogLevel::Info};

inline void set_log_level(LogLevel level) {
    threshold = level;
}

namespace detail {

[[nodiscard]] inline constexpr std::string_view shorten_file_name(std::string_view path) {
    auto idx = path.find("src");

    if (idx == std::string::npos) {
        return path;
    }

    return std::string_view{path.begin() + idx, path.end()};
}

GEN_LOG_LEVEL_FUNC(debug, LogLevel::Debug, stdout, light_green, "DEBUG  ")
GEN_LOG_LEVEL_FUNC(trace, LogLevel::Trace, stdout, light_golden_rod_yellow, "TRACE  ")
GEN_LOG_LEVEL_FUNC(verbose, LogLevel::Verbose, stdout, wheat, "VERBOSE")
GEN_LOG_LEVEL_FUNC(info, LogLevel::Info, stdout, sky_blue, "INFO   ")
GEN_LOG_LEVEL_FUNC(warn, LogLevel::Warning, stderr, orange, "WARN   ")
GEN_LOG_LEVEL_FUNC(error, LogLevel::Error, stderr, indian_red, "ERROR  ")
GEN_LOG_LEVEL_FUNC(fatal, LogLevel::Fatal, stderr, red, "FATAL  ")

}

#define LOG_FATAL(...)   ::detail::fatal_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...)   ::detail::error_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)    ::detail::warn_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)    ::detail::info_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_VERBOSE(...) ::detail::verbose_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_TRACE(...)   ::detail::trace_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...)   ::detail::debug_impl(__LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)

#define LOG_FLAGS()      LOG_VERBOSE("flags: {}", flags)

#define LOG_FUNCTION_INFO(description) \
    LOG_INFO(description);             \
    LOG_FLAGS()

#define LOG_FUNCTION_VERBOSE(description) \
    LOG_VERBOSE(description);             \
    LOG_FLAGS()

#define LOG_FUNCTION_SUCCESS(description)   \
    if (ret.status_code == StatusCode_Ok) { \
        LOG_VERBOSE(description);           \
    }