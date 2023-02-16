#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../project.h"
#include "../../error.h"
#include "../../id.h"
#include "attributetype.h"

struct Attribute;

enum IkarusAttributeDeleteV1Flags {};

bool ikarus_attribute_delete_v1(Project * project, Id attribute, IkarusAttributeDeleteV1Flags flags, ErrorCode * err_out);

enum IkarusAttributeGetBlueprintV1Flags {};

Id ikarus_attribute_get_blueprint_v1(Project* project, Id attribute, IkarusAttributeGetBlueprintV1Flags flags, ErrorCode * err_out);

enum IkarusAttributeGetTypeV1Flags {};

AttributeType ikarus_attribute_get_type_v1(Project* project, Id attribute, IkarusAttributeGetTypeV1Flags flags, ErrorCode * err_out);

