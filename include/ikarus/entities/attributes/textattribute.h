#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../../project.h"
#include "../../status.h"
#include "../../id.h"

enum IkarusTextAttributeCreateV1Flags {
    IkarusTextAttributeCreateV1Flags_None = 0,
};

struct IkarusTextAttributeCreateResult {
    Id attribute;
    StatusCode status_code;
};

IkarusTextAttributeCreateResult ikarus_text_attribute_create_v1(Project * project, Id blueprint, Id parent_folder, size_t position, char const* default_value, IkarusTextAttributeCreateV1Flags flags);

enum IkarusTextAttributeGetDefaultValueV1Flags {
    IkarusTextAttributeGetDefaultValueV1Flags_None = 0,
};

struct IkarusTextAttributeGetDefaultValueResult {
    char const* default_value;
    StatusCode status_code;
};

IkarusTextAttributeGetDefaultValueResult ikarus_text_attribute_get_default_value_v1(Project * project, Id attribute, IkarusTextAttributeGetDefaultValueV1Flags flags);

enum IkarusTextAttributeSetDefaultValueV1Flags {
    IkarusTextAttributeSetDefaultValueV1Flags_None = 0,
};

struct IkarusTextAttributeSetDefaultValueResult {
    StatusCode status_code;
};

IkarusTextAttributeSetDefaultValueResult ikarus_text_attribute_set_default_value_v1(Project * project, Id attribute, char const* new_default_value, IkarusTextAttributeSetDefaultValueV1Flags flags);

enum IkarusTextAttributeGetValueV1Flags {
    IkarusTextAttributeGetValueV1Flags_None = 0,
};

struct IkarusTextAttributeGetValueResult {
    char const* value;
    StatusCode status_code;
};

IkarusTextAttributeGetValueResult ikarus_text_attribute_get_value_v1(Project * project, Id attribute, Id instance, IkarusTextAttributeGetValueV1Flags flags);

enum IkarusTextAttributeSetValueV1Flags {
    IkarusTextAttributeSetValueV1Flags_None = 0,
};

struct IkarusTextAttributeSetValueResult {
    StatusCode status_code;
};

IkarusTextAttributeSetValueResult ikarus_text_attribute_set_value_v1(Project * project, Id attribute, Id instance, char const* new_value, IkarusTextAttributeSetValueV1Flags flags);

enum IkarusTextAttributeGetValuesV1Flags {
    IkarusTextAttributeGetValuesV1Flags_None = 0,
};

struct IkarusTextAttributeGetValuesResult {
    size_t count;
    StatusCode status_code;
};

IkarusTextAttributeGetValuesResult ikarus_text_attribute_get_values_v1(Project * project, char const** values_out, size_t values_out_size, IkarusTextAttributeGetValuesV1Flags flags);

