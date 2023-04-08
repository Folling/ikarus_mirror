#pragma once

#include "coded/db/database.hpp"
#include "ikarus/id.h"
#include "util/logger.hpp"
#include "util/structs/result.hpp"
#include "validation/arg.hpp"

#include <cstdint>
#include <string_view>

namespace util {

inline Option<Id> db_id(Id id) {
    if (id == ID_NULL) {
        return none();
    } else {
        return some(id);
    }
}

inline char * fetch_single_string(
    DbHandle const& db_handle,
    Id entity,
    std::string_view table,
    std::string_view column,
    std::string_view id_column,
    StatusCode * status_out
) {
    CHECK(
        nullptr,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(db_handle, entity, "entity", status_out)
    );

    VTRYRV(
        auto str,
        nullptr,
        db_handle.get_db()->get_one<db::RawString>(
            status_out, fmt::format("SELECT `{}` FROM `{}` WHERE `{}` = ?", column, table, id_column), entity
        )
    );

    if (str.data == nullptr) {
        LOG_ERROR("{} fetched from db is null", column);
        RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
    }

    char * ret = new char[str.size];

    std::strncpy(ret, str.data, str.size);

    return ret;
}

template<int extraValidationFlags>
inline bool update_single_string(
    DbHandle const& db_handle,
    Id entity,
    std::string_view table,
    std::string_view column,
    std::string_view id_column,
    char const * new_value,
    StatusCode * status_out
) {
    CHECK(
        false,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(db_handle, entity, "entity", status_out)
    );

    CHECK(false, validation::validate_string<validation::NotNull | extraValidationFlags>(new_value, "entity", status_out));

    TRYRV(
        false,
        db_handle.get_db()->exec(
            status_out, fmt::format("UPDATE `{}` SET `{}` = ? WHERE `{}` = ?", table, column, id_column), new_value, entity
        )
    );

    LOG_VERBOSE("successfully updated {}", column);

    return true;
}

inline Result<void, int> create_entity(
    DbHandle const& db_handle,
    Id entity,
    Option<Id> scope,
    Id parent_folder,
    std::size_t position,
    std::string_view name,
    std::string_view information,
    StatusCode * status_out
) {
    LOG_VERBOSE("creating entities entry");
    TRY(db_handle.get_db()->exec(
        status_out, "INSERT INTO `entities`(`id`, `name`, `information`) VALUES(?, ?, ?)", entity, name, information
    ));

    LOG_VERBOSE("updating siblings' positions");
    TRY(db_handle.get_db()->exec(
        status_out,
        "UPDATE `entity_tree` SET `position` = `position` + 1 WHERE `scope` = ? AND `parent_id` = ? AND `position` >= ?",
        scope,
        util::db_id(parent_folder),
        position
    ));

    LOG_VERBOSE("creating tree entry");
    TRY(db_handle.get_db()->exec(
        status_out,
        "INSERT INTO `entity_tree`(`entity_id`, `scope`, `parent_id`, `position`) VALUES(?, ?, ?, ?)",
        entity,
        scope,
        util::db_id(parent_folder),
        position
    ));

    return ok();
}

}
