#pragma once

#include <gmp.h>

#include <concepts>
#include <filesystem>
#include <type_traits>

#include <nlohmann/json.hpp>

#include <unicode/numberformatter.h>
#include <unicode/utf8.h>

#include <cppbase/strings.hpp>

#include <ikarus/entities/property_type.h>
#include <ikarus/entities/toggle_value.h>
#include <ikarus/id.h>
#include <ikarus/path.h>

#include <impl/project.hpp>

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

bool validate_exists(Project const * project, Id id) {
    return project->get_db().get_one<bool>("SELECT EXISTS(SELECT 1 FROM `entities` WHERE `id` = ?", id).unwrap_value_or(false);
}

bool validate_position_within_bounds(Project const * project, std::size_t idx, Id folder) {
    return project->get_db().get_one<bool>("SELECT ? < COUNT(*) FROM `entity_tree` WHERE `parent_id` = ?", folder).unwrap_value_or(false);
}

bool validate_is(Id entity, int types) {
    // conditions are OR'd, so we need to check all possible values and can only return if we find one that matches
    for (int i = EntityType_First; i < EntityType_Max; i <<= 1) {
        if ((types & (1 << i)) != 0) {
            if (id_get_entity_type(entity) == i) {
                return true;
            }
        }
    }

    return false;
}

bool validate_path(char const * _path) {
    // honestly I'm not sure how we would validate this given that it can apparently be filesystem dependent
    return true;
}

bool validate_path(Path path) {
    return validate_path(path.data);
}

bool validate_utf8(char const * str) {
    auto length = std::strlen(str);

    for (int32_t i = 0; i < length;) {
        UChar32 c;
        U8_NEXT(str, i, length, c);

        if (c < 0) {
            return false;
        }
    }

    return true;
}

bool validate_utf8(Path path) {
    return validate_utf8(path.data);
}

bool validate_not_blank(char const * str) {
    return !cppbase::is_empty_or_blank(str);
}

bool validate_path_exists(Path path) {
    std::error_code ec;
    if (!std::filesystem::exists(path.data, ec)) {
        return false;
    }

    return !static_cast<bool>(ec);
}

bool validate_path_parent_exists(Path path) {
    std::filesystem::path test{path.data};

    std::error_code ec;
    if (!exists(test.parent_path(), ec) || ec) {
        return false;
    }

    return true;
}

bool validate_property_value(PropertyType type, char const * value, nlohmann::json const& _settings) {
    auto json = nlohmann::json::parse(value, nullptr, false);

    if (json.is_discarded()) {
        return false;
    }

    if (!json.contains("data")) {
        return false;
    }

    auto const& data = json.at("data");

    if (!data.is_array()) {
        return false;
    }

    auto const& array = data.get<nlohmann::json::array_t>();

    switch (type) {
    case PropertyType_Toggle: {
        for (auto const& array_value : array) {
            if (!array_value.is_string()) {
                return false;
            }

            auto number_value = array_value.get<nlohmann::json::number_unsigned_t>();

            if (number_value != ToggleValue_No && number_value != ToggleValue_Maybe && number_value != ToggleValue_Yes) {
                return false;
            }
        }

        break;
    }
    case PropertyType_Number: {
        auto formatter = icu::number::NumberFormatter::with().locale(icu::Locale::getDefault());
        UErrorCode ec = U_ZERO_ERROR;

        for (auto const& array_value : array) {
            if (!array_value.is_string()) {
                return false;
            }

            auto string_value = array_value.get<nlohmann::json::string_t>();

            formatter.formatDecimal(string_value, ec);

            if (U_FAILURE(ec)) {
                return false;
            }
        }

        break;
    }
    case PropertyType_Text: {
        for (auto const& array_value : array) {
            if (!array_value.is_string()) {
                return false;
            }
        }

        break;
    }
    }

    return true;
}

bool validate_property_value_db(Project const * project, Id property, char const * value) {
    VTRYRV(int type, false, project->get_db().get_one<int>("SELECT `type` FROM `properties` WHERE `id` = ?", property));
    VTRYRV(
        std::string settings, false, project->get_db().get_one<std::string>("SELECT `settings` FROM `properties` WHERE `id` = ?", property)
    );

    auto settings_json = nlohmann::json::parse(settings, nullptr, false);

    if (settings_json.is_discarded()) {
        return false;
    }

    return validate_property_value(static_cast<PropertyType>(type), value, settings_json);
}
