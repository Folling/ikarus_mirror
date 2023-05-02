#pragma once

#include <nlohmann/json.hpp>

#include <ikarus/types/property_type.h>

[[nodiscard]] nlohmann::json property_type_get_default_value(PropertyType type);