#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../../project.h"
#include "../../status.h"
#include "../../id.h"
#include "attributetype.h"

enum IkarusAttributeDeleteV1Flags {
    IkarusAttributeDeleteV1Flags_None = 0,
};

struct IkarusAttributeDeleteResult {
    StatusCode status_code;
};

IkarusAttributeDeleteResult ikarus_attribute_delete_v1(Project * project, Id attribute, IkarusAttributeDeleteV1Flags flags);

enum IkarusAttributeGetBlueprintV1Flags {
    IkarusAttributeGetBlueprintV1Flags_None = 0,
};

struct IkarusAttributeGetBlueprintResult {
    Id blueprint;
    StatusCode status_code;
};

IkarusAttributeGetBlueprintResult ikarus_attribute_get_blueprint_v1(Project* project, Id attribute, IkarusAttributeGetBlueprintV1Flags flags);

enum IkarusAttributeGetTypeV1Flags {
    IkarusAttributeGetTypeV1Flags_None = 0,
};

struct IkarusAttributeGetTypeResult {
    AttributeType type;
    StatusCode status_code;
};

IkarusAttributeGetTypeResult ikarus_attribute_get_type_v1(Project* project, Id attribute, IkarusAttributeGetTypeV1Flags flags);

