#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../project.h"
#include "../error.h"
#include "../id.h"

struct Blueprint;

enum IkarusBlueprintCreateV1Flags {};

Id ikarus_blueprint_create_v1(Project * project, Id parent_folder, char const * name, IkarusBlueprintCreateV1Flags flags, ErrorCode * err_out);

enum IkarusBlueprintDeleteV1Flags {};

bool ikarus_blueprint_delete_v1(Project * project, Id blueprint, IkarusBlueprintDeleteV1Flags flags, ErrorCode * err_out);

enum IkarusBlueprintGetAttributesV1Flags {};

bool ikarus_blueprint_get_attributes_v1(Project * project, Id blueprint, Id* attributes_out, size_t attributes_out_size, IkarusBlueprintGetAttributesV1Flags flags, ErrorCode * err_out);

enum IkarusBlueprintGetAttributesCountV1Flags {};

size_t ikarus_blueprint_get_attributes_count_v1(Project * project, Id blueprint, IkarusBlueprintGetAttributesCountV1Flags flags, ErrorCode * err_out);

enum IkarusBlueprintGetInstancesV1Flags {};

bool ikarus_blueprint_get_instances_v1(Project * project, Id blueprint, Id* instances_out, size_t instances_out_size, IkarusBlueprintGetInstancesV1Flags flags, ErrorCode * err_out);

enum IkarusBlueprintGetInstancesCountV1Flags {};

size_t ikarus_blueprint_get_instances_count_v1(Project * project, Id blueprint, IkarusBlueprintGetInstancesCountV1Flags flags, ErrorCode * err_out);

