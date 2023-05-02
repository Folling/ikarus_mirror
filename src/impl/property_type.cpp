#include "property_type.hpp"

nlohmann::json property_type_get_default_value(PropertyType type, nlohmann::json const& _settings) {
    switch (type) {
    case PropertyType_Toggle: {
        return {
            {"data", nlohmann::json::array({0})}
        };
    };
    case PropertyType_Number: {
        return {
            {"data", nlohmann::json::array({"0.0"})}
        };
    }
    case PropertyType_Text: {
        return {
            {"data", nlohmann::json::array({""})}
        };
    }
    }
}