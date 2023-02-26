#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"

enum IkarusBlueprintCreateV1Flags {
    IkarusBlueprintCreateV1Flags_None = 0,
};

struct IkarusBlueprintCreateResult {
    Id blueprint;
    StatusCode status_code;
};

IkarusBlueprintCreateResult ikarus_blueprint_create_v1(Project * project, Id parent_folder, size_t position, char const * name, IkarusBlueprintCreateV1Flags flags);

enum IkarusBlueprintDeleteV1Flags {
    IkarusBlueprintDeleteV1Flags_None = 0,
};

struct IkarusBlueprintDeleteResult {
    StatusCode status_code;
};

IkarusBlueprintDeleteResult ikarus_blueprint_delete_v1(Project * project, Id blueprint, IkarusBlueprintDeleteV1Flags flags);

enum IkarusBlueprintGetAttributesV1Flags {
    IkarusBlueprintGetAttributesV1Flags_None = 0,
};

struct IkarusBlueprintGetAttributesResult {
    size_t count;
    StatusCode status_code;
};

IkarusBlueprintGetAttributesResult ikarus_blueprint_get_attributes_v1(Project * project, Id blueprint, Id* attributes_out, size_t attributes_out_size, IkarusBlueprintGetAttributesV1Flags flags);

enum IkarusBlueprintGetAttributesCountV1Flags {
    IkarusBlueprintGetAttributesCountV1Flags_None = 0,
};

struct IkarusBlueprintGetAttributesCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusBlueprintGetAttributesCountResult ikarus_blueprint_get_attributes_count_v1(Project * project, Id blueprint, IkarusBlueprintGetAttributesCountV1Flags flags);

enum IkarusBlueprintGetInstancesV1Flags {
    IkarusBlueprintGetInstancesV1Flags_None = 0,
};

struct IkarusBlueprintGetInstancesResult {
    size_t count;
    StatusCode status_code;
};

IkarusBlueprintGetInstancesResult ikarus_blueprint_get_instances_v1(Project * project, Id blueprint, Id* instances_out, size_t instances_out_size, IkarusBlueprintGetInstancesV1Flags flags);

enum IkarusBlueprintGetInstancesCountV1Flags {
    IkarusBlueprintGetInstancesCountV1Flags_None = 0,
};

struct IkarusBlueprintGetInstancesCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusBlueprintGetInstancesCountResult ikarus_blueprint_get_instances_count_v1(Project * project, Id blueprint, IkarusBlueprintGetInstancesCountV1Flags flags);

