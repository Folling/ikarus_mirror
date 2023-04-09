#pragma once

#include "ikarus/status.h"

#include <sqlite3.h>

#define RETURN_STATUS_OUT(ret, sc) \
    if (status_out != nullptr) {   \
        *status_out = sc;          \
    }                              \
    return ret

#define RETURN_STATUS(ret, sc) \
    (ret).status_code = sc;    \
    return ret

#define LOG_STD_ERROR(msg, ec)        LOG_ERROR(msg ". Error({}): {}", ec.value(), ec.message())
#define LOG_STD_ERROR_F(msg, ec, ...) LOG_ERROR(msg ". Error({}): {}", __VA_ARGS__, ec.value(), ec.message())
