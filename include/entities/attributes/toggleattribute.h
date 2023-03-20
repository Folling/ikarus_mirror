#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../project.h"
#include "../../error.h"
#include "../../id.h"
#include "values/togglevalue.h"

struct ToggleAttribute;

enum IkarusToggleAttributeCreateV1Flags {};

Id ikarus_toggle_attribute_create_v1(Project * project, Id blueprint, ToggleValue default_value, IkarusToggleAttributeCreateV1Flags flags, ErrorCode * err_out);

enum IkarusToggleAttributeSetDefaultValueV1Flags {};

bool ikarus_toggle_attribute_set_default_value_v1(Project * project, Id attribute, ToggleValue new_default_value, IkarusToggleAttributeSetDefaultValueV1Flags flags, ErrorCode * err_out);

enum IkarusToggleAttributeGetValueV1Flags {};

ToggleValue ikarus_toggle_attribute_get_value_v1(Project * project, Id attribute, Id instance, IkarusToggleAttributeGetValueV1Flags flags, ErrorCode * err_out);

enum IkarusToggleAttributeSetValueV1Flags {};

bool ikarus_toggle_attribute_set_value_v1(Project * project, Id attribute, Id instance, ToggleValue new_value, IkarusToggleAttributeSetValueV1Flags flags, ErrorCode * err_out);

enum IkarusToggleAttributeGetValuesV1Flags {};

bool ikarus_toggle_attribute_get_values_v1(Project * project, ToggleValue* values_out, size_t values_out_size, IkarusToggleAttributeGetValuesV1Flags flags, ErrorCode * err_out);

