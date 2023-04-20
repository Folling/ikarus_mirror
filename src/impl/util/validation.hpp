#pragma once

#include <concepts>
#include <type_traits>

#include <ikarus/entities/property_type.h>
#include <ikarus/id.h>
#include <ikarus/path.h>

template<typename T>
bool validate_not_null(T const * ptr) {
    return ptr != nullptr;
}

bool validate_not_null(Path path) {
    return path.data != nullptr;
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

bool validate_position_within_bounds(std::size_t idx, Id folder) {
    return true;  // TODO
}

bool validate_is(Id entity, EntityTypes types) {
    // conditions are or'd, so we need to check all possible values and can only return if we find one that matches
    for (int i = EntityType_First; i < EntityType_Max; i <<= 1) {
        if ((types & (1 << i)) != 0) {
            if (id_get_entity_type(entity) == i) {
                return true;
            }
        }
    }

    return false;
}

bool validate_path(char const * path) {}

bool validate_path(Path path) {
    return validate_path(path.data);
}

bool validate_utf8(char const * str) {}

bool validate_utf8(Path path) {
    return validate_utf8(path.data);
}

bool validate_not_blank(char const * str) {}

bool validate_path_parent_exists(Path path) {}

bool validate_property_value(PropertyType type, char const * value) {}

bool validate_property_value_db(Id property, char const * value) {}
