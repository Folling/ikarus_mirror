#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../project.h"
#include "../../error.h"
#include "../../id.h"

struct NumberAttribute;

enum IkarusNumberAttributeCreateStrV1Flags {};

Id ikarus_number_attribute_create_str_v1(Project * project, Id blueprint, char const* default_value, IkarusNumberAttributeCreateStrV1Flags flags, ErrorCode * err_out);

enum IkarusNumberAttributeSetDefaultValueStrV1Flags {};

bool ikarus_number_attribute_set_default_value_str_v1(Project * project, Id attribute, char const* new_default_value, IkarusNumberAttributeSetDefaultValueStrV1Flags flags, ErrorCode * err_out);

enum IkarusNumberAttributeGetValueStrV1Flags {};

char const* ikarus_number_attribute_get_value_str_v1(Project * project, Id attribute, Id instance, IkarusNumberAttributeGetValueStrV1Flags flags, ErrorCode * err_out);

enum IkarusNumberAttributeSetValueStrV1Flags {};

bool ikarus_number_attribute_set_value_str_v1(Project * project, Id attribute, Id instance, char const* new_value, IkarusNumberAttributeSetValueStrV1Flags flags, ErrorCode * err_out);

enum IkarusNumberAttributeGetValuesStrV1Flags {};

bool ikarus_number_attribute_get_values_str_v1(Project * project, char const** values_out, size_t values_out_size, IkarusNumberAttributeGetValuesStrV1Flags flags, ErrorCode * err_out);

