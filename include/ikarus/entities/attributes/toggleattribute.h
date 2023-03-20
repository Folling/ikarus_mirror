#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../../project.h"
#include "../../status.h"
#include "../../id.h"
#include "values/togglevalue.h"

enum IkarusToggleAttributeCreateV1Flags {
    IkarusToggleAttributeCreateV1Flags_None = 0,
};

struct IkarusToggleAttributeCreateResult {
    Id attribute;
    StatusCode status_code;
};

IkarusToggleAttributeCreateResult ikarus_toggle_attribute_create_v1(Project * project, Id blueprint, Id parent_folder, size_t position, ToggleValue default_value, IkarusToggleAttributeCreateV1Flags flags);

enum IkarusToggleAttributeGetDefaultValueV1Flags {
    IkarusToggleAttributeGetDefaultValueV1Flags_None = 0,
};

struct IkarusToggleAttributeGetDefaultValueResult {
    ToggleValue default_value;
    StatusCode status_code;
};

IkarusToggleAttributeGetDefaultValueResult ikarus_toggle_attribute_get_default_value_v1(Project * project, Id attribute, ToggleValue new_default_value, IkarusToggleAttributeGetDefaultValueV1Flags flags);

enum IkarusToggleAttributeSetDefaultValueV1Flags {
    IkarusToggleAttributeSetDefaultValueV1Flags_None = 0,
};

struct IkarusToggleAttributeSetDefaultValueResult {
    StatusCode status_code;
};

IkarusToggleAttributeSetDefaultValueResult ikarus_toggle_attribute_set_default_value_v1(Project * project, Id attribute, ToggleValue new_default_value, IkarusToggleAttributeSetDefaultValueV1Flags flags);

enum IkarusToggleAttributeGetValueV1Flags {
    IkarusToggleAttributeGetValueV1Flags_None = 0,
};

struct IkarusToggleAttributeGetValueResult {
    ToggleValue value;
    StatusCode status_code;
};

IkarusToggleAttributeGetValueResult ikarus_toggle_attribute_get_value_v1(Project * project, Id attribute, Id instance, IkarusToggleAttributeGetValueV1Flags flags);

enum IkarusToggleAttributeSetValueV1Flags {
    IkarusToggleAttributeSetValueV1Flags_None = 0,
};

struct IkarusToggleAttributeSetValueResult {
    StatusCode status_code;
};

IkarusToggleAttributeSetValueResult ikarus_toggle_attribute_set_value_v1(Project * project, Id attribute, Id instance, ToggleValue new_value, IkarusToggleAttributeSetValueV1Flags flags);

enum IkarusToggleAttributeGetValuesV1Flags {
    IkarusToggleAttributeGetValuesV1Flags_None = 0,
};

struct IkarusToggleAttributeGetValuesResult {
    size_t count;
    StatusCode status_code;
};

IkarusToggleAttributeGetValuesResult ikarus_toggle_attribute_get_values_v1(Project * project, ToggleValue* values_out, size_t values_out_size, IkarusToggleAttributeGetValuesV1Flags flags);

