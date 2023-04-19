#pragma once

#include <filesystem>
#include <string_view>

#include <ikarus/macros.h>
#include <impl/util/structs/result.hpp>

#if defined(IKA_OS_FAMILY_UNIX)
#include <fcntl.h>

#include <cstdio>
#elif defined(IKA_OS_WIN)
#include <windows.h>
#endif

namespace os::fs {

namespace fs = std::filesystem;

[[nodiscard]] Result<fs::path, int> get_temp_directory() {
    fs::path path{};
    if (std::error_code rc; path = fs::temp_directory_path(rc), rc) {
        LOG_ERROR("unable to fetch temp directory path: {}", rc.value());
        return err(rc.value());
    }

    return ok(std::move(path));
}

[[nodiscard]] Result<fs::path, int> get_temp_file(std::string_view filename) {
    VTRY(auto temp_dir, get_temp_directory());

#if defined(IKA_OS_FAMILY_UNIX)
    auto template_path = fmt::format("{}/{}XXXXXX", temp_dir.c_str(), filename);
    char const * ret = mktemp(template_path.data());
    if (ret == nullptr) {
        int rc = errno;
        LOG_ERROR("unable to generate unique temporary filename. Error {}: {}", rc, strerror(rc));
        return err(rc);
    }
    return ok(ret);
#elif defined(IKA_OS_WIN)
    std::string path;
    path.resize(MAX_PATH);
    int rc = GetTempFileName(temp_dir.c_str(), TEXT(filename.data()), 0, path.data());

    if (rc == 0) {
        LPVOID err_msg;
        DWORD err = GetLastError();
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&err_msg,
            0,
            nullptr
        );
        LOG_ERROR("unable to generate unique temporary filename. Error {}: {}", err_msg);
        LocalFree(err_msg);
    }

    return ok(path);
#else
#error "unknown os for temporary file detection"
#endif
}

}