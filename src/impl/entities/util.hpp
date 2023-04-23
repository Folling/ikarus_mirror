#pragma once

#include <cstdint>
#include <string_view>

#include <cppbase/logger.hpp>
#include <cppbase/result.hpp>

#include <sqlitecpp/database.hpp>

#include <ikarus/id.h>

#include <impl/db/util.hpp>
#include <impl/util/format.hpp>

namespace util {

inline cppbase::Result<void, int> create_entity(
    sqlitecpp::Database * db, Id entity, Id parent_folder, std::size_t position, std::string_view name, std::string_view information
) {
    LOG_VERBOSE("creating entities entry");
    TRY(db->exec("INSERT INTO `entities`(`id`, `name`, `information`) VALUES(?, ?, ?)", entity, name, information));

    LOG_VERBOSE("updating siblings' positions");
    TRY(db->exec("UPDATE `entity_tree` SET `position` = `position` + 1 WHERE `parent_id` = ? AND `position` >= ?", parent_folder, position)
    );

    LOG_VERBOSE("creating tree entry");
    TRY(db->exec(
        "INSERT INTO `entity_tree`(`entity_id`, `scope`, `parent_id`, `position`) VALUES(?, ?, ?, ?)", entity, parent_folder, position
    ));

    return cppbase::ok();
}

}
