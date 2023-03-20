#pragma once

#include <cstdint>
#include <string_view>

#include <db/db.hpp>
#include <ikarus/id.h>
#include <util/logger.hpp>
#include <util/structs/result.hpp>
#include <validation/arg.hpp>

namespace util {

std::optional<Id> db_id(Id id) {
    if (id_is_null(id)) {
        return std::nullopt;
    } else {
        return std::make_optional(id);
    }
}

char * fetch_single_string(
    Project * project, Id entity, std::string_view table, std::string_view column, std::string_view id_column, StatusCode * status_out
) {
    CHECK(nullptr, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", status_out));

    CHECK(
        nullptr,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(project, entity, "entity", status_out)
    );

    VTRYRV(
        auto str,
        nullptr,
        db::get_one<db::RawString>(
            project->db, status_out, fmt::format("SELECT `{}` FROM `{}` WHERE `{}` = ?", column, table, id_column), entity
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

bool update_single_string(
    Project * project,
    Id entity,
    std::string_view table,
    std::string_view column,
    std::string_view id_column,
    char const * new_value,
    StatusCode * status_out
) {
    CHECK(false, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", status_out));

    CHECK(
        false, validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(project, entity, "entity", status_out)
    );

    CHECK(false, validation::validate_string<validation::NotNull>(new_value, "entity", status_out));

    TRYRV(
        false,
        db::exec(
            project->db, status_out, fmt::format("UPDATE `{}` SET `{}` = ? WHERE `{}` = ?", table, column, id_column), new_value, entity
        )
    );

    LOG_VERBOSE("successfully updated {}", column);

    return true;
}

Result<void, int> create_entity(
    sqlite3 * db,
    Id entity,
    Id parent_folder,
    std::size_t position,
    std::string_view name,
    std::string_view information,
    StatusCode * status_out
) {
    LOG_VERBOSE("creating entities entry");
    TRY(db::exec(db, status_out, "INSERT INTO `entities`(`id`, `name`, `information`) VALUES(?, ?, ?)", entity, name, information));

    LOG_VERBOSE("creating tree entry");
    TRY(db::exec(
        db, status_out, "INSERT INTO `entity_tree`(`entity_id`, `parent_id`, `position`) VALUES(?, ?, ?)", entity, parent_folder, position
    ));

    return ok();
}

}
