#pragma once

#include <optional>

#include <fmt/format.h>

template<typename T>
struct fmt::formatter<std::optional<T>> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(std::optional<T> const& opt, fmt::format_context& ctx) const {
        if (opt.has_value()) {
            return fmt::format_to(ctx.out(), "{}", opt.value());
        } else {
            return fmt::format_to(ctx.out(), "nullopt");
        }
    }
};
