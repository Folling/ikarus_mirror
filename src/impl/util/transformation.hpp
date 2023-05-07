#pragma once

#include <nlohmann/json.hpp>

#include <cppbase/logger.hpp>
#include <cppbase/option.hpp>

#include <ikarus/types/property_settings.h>
#include <ikarus/types/value.h>

namespace util {

using cppbase::Option;
using cppbase::some;

[[nodiscard]] inline Option<nlohmann::json> transform(PropertySettings value) {
    try {
        return some(nlohmann::json::parse(value.data));
    } catch (nlohmann::json::parse_error const& e) {
        LOG_ERROR("unable to parse property settings as JSON: {}", e.what());
        return {};
    }
}

[[nodiscard]] inline Option<nlohmann::json> transform(Value value) {
    try {
        return some(nlohmann::json::parse(value.data));
    } catch (nlohmann::json::parse_error const& e) {
        LOG_ERROR("unable to parse ikarus value as JSON: {}", e.what());
        return {};
    }
}

template<typename T>
[[nodiscard]] Option<T> transform(T const& arg) {
    return some(arg);
}
}
