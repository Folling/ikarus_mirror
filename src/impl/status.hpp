#pragma once

#include <ikarus/status.h>

#define TRY_STATUS()

#define RETURN_STATUS_OUT(ret, sc) \
    if (status_out != nullptr) {   \
        *status_out = sc;          \
    }                              \
    return ret

#define RETURN_STATUS(ret, sc) \
    (ret).status_code = sc;    \
    return ret
