#pragma once

#include <unicode/uchar.h>
#include <unicode/uiter.h>

#include <ikarus/id.h>
#include <ikarus/project.h>
#include <ikarus/status.h>
#include <impl/project.hpp>
#include <impl/status.hpp>
#include <impl/util/string.hpp>
#include <impl/util/structs/result.hpp>
#include <impl/util/templates.hpp>
#include <sqlitecpp/database.hpp>

#define CHECK(ret, ...)   \
    if (!(__VA_ARGS__)) { \
        return ret;       \
    }

namespace validation {

enum ValidationFlags { None = 0, Type = 1 << 0, NotNull = 1 << 1, NotEmpty = 1 << 2, Exists = 1 << 3, DoesntExist = 1 << 4 };

template<int F>
bool validate_path(char const * path, std::string_view ident, StatusCode * status_out) {
    static_assert((F & ~(NotNull | Exists | DoesntExist)) == 0, "path validation can only use NotNull, Exists, and DoesntExist");

    if constexpr ((F & NotNull) != 0) {
        LOG_VERBOSE("validating that {} isn't null", ident);

        if (path == nullptr) {
            LOG_ERROR("{} is null", ident);
            RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
        }
    } else {
        if (path == nullptr) {
            LOG_VERBOSE("{} is null, but that's fine. Skipping further validation", ident);
            return true;
        }
    }

    LOG_VERBOSE("{} = {}", ident, path);

    if constexpr ((F & Exists) != 0 || (F & DoesntExist) != 0) {
        static_assert((F & Exists) + (F & DoesntExist) != (Exists + DoesntExist), "paths aren't in superposition");

        std::error_code ec;
        bool exists = std::filesystem::exists(path, ec);

        if (ec) {
            LOG_STD_ERROR_F("unable to check whether {} exists", ec, ident);
            RETURN_STATUS_OUT(false, StatusCode_InternalError);
        }

        if constexpr ((F & Exists) != 0) {
            if (exists) {
                LOG_VERBOSE("{} should exist and does", ident);
            } else {
                LOG_ERROR("{} should exist but doesn't", ident);
                RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
            }
        } else if constexpr ((F & DoesntExist) != 0) {
            if (!exists) {
                LOG_VERBOSE("{} shouldn't exist and doesn't", ident);
            } else {
                LOG_ERROR("{} shouldn't exist but does", ident);
                RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
            }
        }
    }

    LOG_VERBOSE("successfully validated {}", ident);

    return true;
}

template<int F, typename T>
bool validate_pointer(T const * pointer, std::string_view ident, StatusCode * status_out) {
    static_assert((F & ~(NotNull)) == 0, "pointer validation can only use NotNull");

    LOG_VERBOSE("validating {}", ident);

    if constexpr ((F & NotNull) != 0) {
        LOG_VERBOSE("validating that {} isn't null", ident);
        if (pointer == nullptr) {
            LOG_ERROR("{} is null", ident);
            RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
        }
    } else {
        if (pointer == nullptr) {
            LOG_VERBOSE("{} is null, but that's fine. Skipping further validation", ident);
            return true;
        }
    }

    LOG_VERBOSE("successfully validated {}", ident);

    return true;
}

template<int F>
bool validate_string(char const * string, std::string_view ident, StatusCode * status_out) {
    static_assert((F & ~(NotNull | NotEmpty)) == 0, "string validation can only use NotNull");

    LOG_VERBOSE("validating {}", ident);

    if constexpr ((F & NotNull) != 0) {
        LOG_VERBOSE("validating that {} isn't null", ident);
        if (string == nullptr) {
            LOG_ERROR("{} is null", ident);
            RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
        }
    } else {
        if (string == nullptr) {
            LOG_VERBOSE("{} is null, but that's fine. Skipping further validation", ident);
            return true;
        }
    }

    LOG_VERBOSE("{} = {}", ident, string);

    if constexpr ((F & NotEmpty) != 0) {
        LOG_VERBOSE("validating that {} isn't empty", ident);
        if (str::is_empty_or_blank(string)) {
            LOG_ERROR("{} was blank/empty", ident);
            return false;
        }
    }

    LOG_VERBOSE("successfully validated {}", ident);

    return true;
}

template<int F>
bool validate_project(Project const * project, std::string_view ident, StatusCode * status_out) {
    static_assert((F & ~(NotNull | Exists | DoesntExist)) == 0, "project validation can only use NotNull, Exists and DoesntExist");

    LOG_VERBOSE("validating {}", ident);

    if constexpr ((F & NotNull) != 0) {
        LOG_VERBOSE("validating that {} isn't null", ident);

        if (project == nullptr) {
            LOG_ERROR("{} is null", ident);
            RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
        }
    } else {
        if (project == nullptr) {
            LOG_VERBOSE("{} is null, but that's fine. Skipping further validation", ident);
            return true;
        }
    }

    LOG_VERBOSE("{} = {}", ident, project->get_path().c_str());

    if constexpr (F & Exists || F & DoesntExist) {
        static_assert((F & Exists) + (F & DoesntExist) != (Exists + DoesntExist), "projects aren't in superposition");

        LOG_VERBOSE("validating project path's existence");

        // clang-format off
        if (!validate_path<F & (Exists | DoesntExist)>(project->get_path().c_str(), "project path", status_out)) {
            return false;
        }
        // clang-format on
    }

    LOG_VERBOSE("successfully validated {}", ident);

    return true;
}

template<int F, EntityType type>
    requires((F & ~NotNull & ~Type & ~Exists & ~DoesntExist) == 0)
bool validate_entity(DbHandle const& db_handle, Id entity, std::string_view ident, StatusCode * status_out) {
    static_assert(
        (F & ~(NotNull | Type | Exists | DoesntExist)) == 0, "entity validation can only use NotNull, Type, Exists and DoesntExist"
    );

    LOG_VERBOSE("validating {}", ident);

    if constexpr ((F & NotNull) != 0) {
        LOG_VERBOSE("validating that {} isn't null", ident);

        if (entity == ID_NONE) {
            LOG_ERROR("{} is null", ident);
            RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
        }
    } else {
        if (entity == ID_NONE) {
            LOG_VERBOSE("{} is null, but that's fine. Skipping further validation", ident);
            return true;
        }
    }

    LOG_VERBOSE("{} = {}", ident, entity);

    if constexpr ((F & Type) != 0) {
        if constexpr (type != EntityType_None) {
            EntityType expected = type;

            LOG_VERBOSE("validating type");

            if (entity == ID_NONE) {
                LOG_VERBOSE("entity is null");
            } else {
                if (auto received = id_get_entity_type(entity); received != expected) {
                    LOG_ERROR("type is {}, expected {}", received, expected);
                    RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
                }

                LOG_VERBOSE("type matches ({})", expected);
            }
        } else {
            static_assert(tmpl::delayed_false_v<decltype(type)>, "must set type parameter when validating type");
        }
    }

    LOG_VERBOSE("{} = {}", ident, entity);

    if constexpr ((F & Exists) != 0 || (F & DoesntExist) != 0) {
        static_assert((F & Exists) + (F & DoesntExist) != (Exists + DoesntExist), "entities aren't in superposition");
        constexpr bool should_exist = F & Exists;

        std::string table{};
        std::string id_column{};

        if constexpr (type != EntityType_None) {
            if constexpr (type == EntityType_Folder) {
                table = "folders";
                id_column = "entity_id";
            } else if constexpr (type == EntityType_Template) {
                table = "templates";
                id_column = "entity_id";
            } else if constexpr (type == EntityType_Property) {
                table = "properties";
                id_column = "entity_id";
            } else if constexpr (type == EntityType_Page) {
                table = "pages";
                id_column = "entity_id";
            } else {
                static_assert(tmpl::delayed_false_v<decltype(type)>, "unknown entity type");
            }
        } else {
            table = "entities";
            id_column = "id";
        }

        std::string query =
            fmt::format("SELECT {}EXISTS (SELECT 1 FROM `{}` WHERE `{}` = ?)", should_exist ? "" : "NOT ", table, id_column);

        LOG_VERBOSE("validating existence");

        VTRYRV(bool exists, false, db_handle.get_db()->get_one<bool>(status_out, query, entity));

        if constexpr (should_exist) {
            if (exists) {
                LOG_VERBOSE("{} should exist and does", ident);
            } else {
                LOG_ERROR("{} should exist but doesn't", ident);
                RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
            }
        } else {
            if (exists) {
                LOG_ERROR("{} shouldn't exist but does", ident);
                RETURN_STATUS_OUT(false, StatusCode_InvalidArgument);
            } else {
                LOG_VERBOSE("{} shouldn't exist and doesn't", ident);
            }
        }
    }

    LOG_VERBOSE("successfully validated {}", ident);

    return true;
}
}
