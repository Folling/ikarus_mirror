#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../project.h"
#include "../error.h"
#include "../id.h"

struct Entity;

enum IkarusEntityGetParentV1Flags {};

Id ikarus_entity_get_parent_v1(Project * project, Id entity, IkarusEntityGetParentV1Flags flags, ErrorCode * err_out);

enum IkarusEntitySetParentV1Flags {};

bool ikarus_entity_set_parent_v1(Project * project, Id entity, Id new_parent, size_t new_idx, IkarusEntitySetParentV1Flags flags, ErrorCode * err_out);

enum IkarusEntityGetIdxV1Flags {};

size_t ikarus_entity_get_idx_v1(Project * project, Id entity, IkarusEntityGetIdxV1Flags flags, ErrorCode * err_out);

enum IkarusEntitySetIdxV1Flags {};

bool ikarus_entity_set_idx_v1(Project * project, Id entity, size_t new_idx, IkarusEntitySetIdxV1Flags flags, ErrorCode * err_out);

