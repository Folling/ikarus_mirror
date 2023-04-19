#pragma once

#include <optional>

#include <fmt/format.h>

#include <impl/ikarus/path.h>
#include <impl/maths.hpp>
#include <impl/templates.hpp>

namespace format {

template<typename... Args>
constexpr std::string_view format_str() {
    // bit hacky, but it's pretty hard to get this efficient otherwise
    constexpr std::string_view data = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}";

    return data.substr(0, (sizeof...(Args) * 2) + (maths::safe_subtract(sizeof...(Args), 1ul) * 2));
}

}

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
