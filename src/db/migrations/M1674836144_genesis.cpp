#include "M1674836144_genesis.hpp"

#include <db/database.hpp>

namespace db {

Result<void, int> M1674836144_genesis::up(db::Database& db) {
    LOG_VERBOSE("creating metadata table");

    TRY(db.exec(
        nullptr,
        "CREATE TABLE `metadata`(\n"
        // INT is important here - INTEGER would make it an alias for the rowid
        "    `key` TEXT PRIMARY KEY,\n"
        "    `value` TEXT\n"
        ") WITHOUT ROWID, STRICT;"
    ));

    return ok();
}

u64 M1674836144_genesis::get_version() const {
    return 1674836144;
}

}
