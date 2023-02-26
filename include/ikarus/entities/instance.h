#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"

enum IkarusInstanceCreateV1Flags {
    IkarusInstanceCreateV1Flags_None = 0,
};

struct IkarusInstanceCreateResult {
    Id instance;
    StatusCode status_code;
};

IkarusInstanceCreateResult ikarus_instance_create_v1(Project * project, Id blueprint, Id parent_folder, size_t position, char const * name, IkarusInstanceCreateV1Flags flags);

enum IkarusInstanceDeleteV1Flags {
    IkarusInstanceDeleteV1Flags_None = 0,
};

struct IkarusInstanceDeleteResult {
    StatusCode status_code;
};

IkarusInstanceDeleteResult ikarus_instance_delete_v1(Project * project, Id instance, IkarusInstanceDeleteV1Flags flags);

enum IkarusInstanceGetBlueprintV1Flags {
    IkarusInstanceGetBlueprintV1Flags_None = 0,
};

struct IkarusInstanceGetBlueprintResult {
    Id blueprint;
    StatusCode status_code;
};

IkarusInstanceGetBlueprintResult ikarus_instance_get_blueprint_v1(Project* project, Id instance, IkarusInstanceGetBlueprintV1Flags flags);

