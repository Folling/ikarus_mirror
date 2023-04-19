#pragma once

#include <concepts>
#include <type_traits>

#include <ikarus/id.h>

template<typename T>
bool validate_not_null(T const * ptr) {
    return ptr != nullptr;
}

bool validate_id_not_null(Id id) {
    return id != ID_NONE && id != ID_UNSPECIFIED;
}

bool validate_id_not_none(Id id) {
    return id != ID_NONE;
}

bool validate_id_specified(Id id) {
    return id != ID_UNSPECIFIED;
}

bool validate_exists(Id id) {
    return true;  // TODO
}

//
// ParameterValidationType::NotNull => {
//                        format!("validate_not_null({})", param.name)
//                    }
//                    ParameterValidationType::IdNotNull => {
//                        format!("validate_id_not_null({})", param.name)
//                    }
//                    ParameterValidationType::IdNotNone => {
//                        format!("validate_id_not_none({})", param.name)
//                    }
//                    ParameterValidationType::IdSpecified => {
//                        format!("validate_id_specified({})", param.name)
//                    }
//                    ParameterValidationType::Exists => format!("validate_exists({})", param.name),
//                    ParameterValidationType::PositionWithinBounds {
//                        bounds_folder_object,
//                    } => format!(
//                        "validate_position_within_bounds({}, {})",
//                        param.name, bounds_folder_object
//                    ),
//                    ParameterValidationType::Is { expected_types } => format!(
//                        "validate_is({}, {})",
//                        param.name,
//                        expected_types
//                            .iter()
//                            .map(|c| format!("EntityTypes_{}", c))
//                            .collect::<Vec<String>>()
//                            .join(" | ")
//                    ),
//                    ParameterValidationType::ValidPath => format!("validate_path({})", param.name),
//                    ParameterValidationType::ValidUtf8 => format!("validate_utf8({})", param.name),
//                    ParameterValidationType::NotBlank => format!("validate_path({})", param.name),
//                    ParameterValidationType::PathParentMustExist => {
//                        format!("validate_path_parent_exists({})", param.name)
//                    }
//                    ParameterValidationType::ValidPropertyValue { type_source } => {
//                        format!("validate_property_value({}, {})", type_source, param.name)
//                    }
//                    ParameterValidationType::ValidPropertyValueDb { property_source } => format!(
//                        "validate_property_value_db({}, {})",
//                        property_source, param.name
//                    ),