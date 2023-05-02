#pragma once

#include "ikarus/types/id.h"
#include "ikarus/types/path.h"

#include <fmt/format.h>

template<>
struct fmt::formatter<Path> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Path const& path, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", path.data);
    }
};

template<>
struct fmt::formatter<Id> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Id id, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", id.value);
    }
};
