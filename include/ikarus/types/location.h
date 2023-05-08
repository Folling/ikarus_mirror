#pragma once

#ifdef __cplusplus
#include <cstdint>

using std::size_t;
#else
#include <stdint.h>
#endif

#include "id.h"

struct Location {
    Id parent_folder;
    size_t position;
};