#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../project.h"
#include "../../error.h"
#include "../../id.h"

struct TextAttribute;

enum IkarusTextAttributeCreateV1Flags {};

Id ikarus_text_attribute_create_v1(Project * project, Id blueprint, char const* default_value, IkarusTextAttributeCreateV1Flags flags, ErrorCode * err_out);

enum IkarusTextAttributeSetDefaultValueV1Flags {};

bool ikarus_text_attribute_set_default_value_v1(Project * project, Id attribute, char const* new_default_value, IkarusTextAttributeSetDefaultValueV1Flags flags, ErrorCode * err_out);

enum IkarusTextAttributeGetValueV1Flags {};

char const* ikarus_text_attribute_get_value_v1(Project * project, Id attribute, Id instance, IkarusTextAttributeGetValueV1Flags flags, ErrorCode * err_out);

enum IkarusTextAttributeSetValueV1Flags {};

bool ikarus_text_attribute_set_value_v1(Project * project, Id attribute, Id instance, char const* new_value, IkarusTextAttributeSetValueV1Flags flags, ErrorCode * err_out);

enum IkarusTextAttributeGetValuesV1Flags {};

bool ikarus_text_attribute_get_values_v1(Project * project, char const** values_out, size_t values_out_size, IkarusTextAttributeGetValuesV1Flags flags, ErrorCode * err_out);

