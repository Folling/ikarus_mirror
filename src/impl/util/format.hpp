#pragma once

#include <fmt/format.h>

#include <ikarus/types/id.h>
#include <ikarus/types/path.h>
#include <ikarus/types/project.h>
#include <ikarus/types/status.h>

#include <impl/project.hpp>

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
struct fmt::formatter<Project> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Project const& path, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", path.handle->get_path().c_str());
    }
};

template<>
struct fmt::formatter<StatusCode> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(StatusCode const& sc, fmt::format_context& ctx) const {
        switch (sc) {
        case StatusCode_Ok: return fmt::format_to(ctx.out(), "Ok");
        case StatusCode_InvalidArgument: return fmt::format_to(ctx.out(), "InvalidArgument");
        case StatusCode_InvalidVersion: return fmt::format_to(ctx.out(), "InvalidVersion");
        case StatusCode_NotFound: return fmt::format_to(ctx.out(), "NotFound");
        case StatusCode_Duplicate: return fmt::format_to(ctx.out(), "Duplicate");
        case StatusCode_InternalError: return fmt::format_to(ctx.out(), "InternalError");
        case StatusCode_Corrupted: return fmt::format_to(ctx.out(), "Corrupted");
        }
    }
};
