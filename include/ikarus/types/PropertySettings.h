#pragma once

struct PropertySettings {
    char const * data;
};

// hello future me, so the plan is to use a "auto arg0_transformed = transform_arg(arg0)" function that is a no-op for
// most situations but a sensible op in cases where some transformation needs to happen anyway, such as the conversion
// of PropertySettings to a JSON object, or Values to a JSON object good night