#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../project.h"
#include "../error.h"
#include "../id.h"

struct Instance;

enum IkarusInstanceCreateV1Flags {};

Id ikarus_instance_create_v1(Project * project, Id blueprint, Id parent_folder, char const * name, IkarusInstanceCreateV1Flags flags, ErrorCode * err_out);

enum IkarusInstanceDeleteV1Flags {};

bool ikarus_instance_delete_v1(Project * project, Id instance, IkarusInstanceDeleteV1Flags flags, ErrorCode * err_out);

enum IkarusInstanceGetBlueprintV1Flags {};

Id ikarus_instance_get_blueprint_v1(Project* project, Id instance, IkarusInstanceGetBlueprintV1Flags flags, ErrorCode * err_out);

