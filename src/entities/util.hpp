#pragma once

#include <cstdint>
#include <string_view>

#include <db/db.hpp>
#include <ikarus/id.h>
#include <util/structs/result.hpp>

namespace util {

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
        db, status_out, "INSERT INTO `tree`(`entity_id`, `parent_id`, `position`) VALUES(?, ?, ?)", entity, parent_folder, position
    ));

    return ok();
}

}
