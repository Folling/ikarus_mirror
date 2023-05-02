#pragma once

#include "ikarus/types/id.h"

struct PagePropertyValue {
    Id page;
    Id property;
    char const * value;
};