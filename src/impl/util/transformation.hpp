#pragma once

#include <type_traits>

#include <nlohmann/json.hpp>

#include <cppbase/logger.hpp>

#include <ikarus/types/property_settings.h>
#include <ikarus/types/value.h>

namespace util {

template<typename T>
struct Transformed {};

template<>
struct Transformed<PropertySettings> {
    typedef nlohmann::json type;
};

template<>
struct Transformed<Value> {
    typedef nlohmann::json type;
};

template<typename T>
struct Transformer {
    typedef T type;

    [[nodiscard]] static T transform(T const& value, bool * success) {
        *success = true;
        return value;
    }
};

struct TransformedValue {
    nlohmann::json data;
};

template<>
struct Transformer<Value> {
    typedef TransformedValue type;

    [[nodiscard]] static TransformedValue transform(Value const& value, bool * success) {
        try {
            auto ret = nlohmann::json::parse(value.data);
            *success = true;
            return TransformedValue{ret};
        } catch (nlohmann::json::parse_error const& e) {
            LOG_ERROR("unable to parse ikarus value as JSON: {}", e.what());
            *success = false;
            return {};
        }
    }
};

struct TransformedPropertySettings {
    nlohmann::json data;
};

template<>
struct Transformer<PropertySettings> {
    typedef TransformedPropertySettings type;

    [[nodiscard]] static TransformedPropertySettings transform(PropertySettings& settings, bool * success) {
        try {
            auto ret = nlohmann::json::parse(settings.data);
            *success = true;
            return TransformedPropertySettings{ret};
        } catch (nlohmann::json::parse_error const& e) {
            LOG_ERROR("unable to parse property settings as JSON: {}", e.what());
            *success = false;
            return {};
        }
    }
};

}
