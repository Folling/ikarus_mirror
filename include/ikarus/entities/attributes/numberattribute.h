#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../../project.h"
#include "../../status.h"
#include "../../id.h"

enum IkarusNumberAttributeCreateNumberAttributeV1Flags {
    IkarusNumberAttributeCreateNumberAttributeV1Flags_None = 0,
};

struct IkarusNumberAttributeCreateNumberAttributeResult {
    Id attribute;
    StatusCode status_code;
};

IkarusNumberAttributeCreateNumberAttributeResult ikarus_number_attribute_create_number_attribute_v1(Project * project, Id blueprint, Id parent_folder, size_t position, char const* default_value, IkarusNumberAttributeCreateNumberAttributeV1Flags flags);

enum IkarusNumberAttributeGetDefaultValueStrV1Flags {
    IkarusNumberAttributeGetDefaultValueStrV1Flags_None = 0,
};

struct IkarusNumberAttributeGetDefaultValueStrResult {
    char const* default_value;
    StatusCode status_code;
};

IkarusNumberAttributeGetDefaultValueStrResult ikarus_number_attribute_get_default_value_str_v1(Project * project, Id attribute, IkarusNumberAttributeGetDefaultValueStrV1Flags flags);

enum IkarusNumberAttributeSetDefaultValueStrV1Flags {
    IkarusNumberAttributeSetDefaultValueStrV1Flags_None = 0,
};

struct IkarusNumberAttributeSetDefaultValueStrResult {
    StatusCode status_code;
};

IkarusNumberAttributeSetDefaultValueStrResult ikarus_number_attribute_set_default_value_str_v1(Project * project, Id attribute, char const* new_default_value, IkarusNumberAttributeSetDefaultValueStrV1Flags flags);

enum IkarusNumberAttributeGetValueStrV1Flags {
    IkarusNumberAttributeGetValueStrV1Flags_None = 0,
};

struct IkarusNumberAttributeGetValueStrResult {
    char const* value;
    StatusCode status_code;
};

IkarusNumberAttributeGetValueStrResult ikarus_number_attribute_get_value_str_v1(Project * project, Id attribute, Id instance, IkarusNumberAttributeGetValueStrV1Flags flags);

enum IkarusNumberAttributeSetValueStrV1Flags {
    IkarusNumberAttributeSetValueStrV1Flags_None = 0,
};

struct IkarusNumberAttributeSetValueStrResult {
    StatusCode status_code;
};

IkarusNumberAttributeSetValueStrResult ikarus_number_attribute_set_value_str_v1(Project * project, Id attribute, Id instance, char const* new_value, IkarusNumberAttributeSetValueStrV1Flags flags);

enum IkarusNumberAttributeGetValuesStrV1Flags {
    IkarusNumberAttributeGetValuesStrV1Flags_None = 0,
};

struct IkarusNumberAttributeGetValuesStrResult {
    size_t count;
    StatusCode status_code;
};

IkarusNumberAttributeGetValuesStrResult ikarus_number_attribute_get_values_str_v1(Project * project, char const** values_out, size_t values_out_size, IkarusNumberAttributeGetValuesStrV1Flags flags);

