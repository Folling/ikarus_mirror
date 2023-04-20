#pragma once

#include "fmt/format.h"
#include "ikarus/path.h"

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
