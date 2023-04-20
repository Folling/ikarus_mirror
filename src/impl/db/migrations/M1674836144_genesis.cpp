#include "M1674836144_genesis.hpp"

#include <cppbase/logger.hpp>
#include <sqlitecpp/database.hpp>

namespace db {

Result<void, int> M1674836144_genesis::up(db::Database& db) {
    LOG_VERBOSE("creating metadata table");

    TRY(
        db.exec("CREATE TABLE `metadata`(\n"
                // INT is important here - INTEGER would make it an alias for the rowid
                "    `key` TEXT PRIMARY KEY,\n"
                "    `value` TEXT\n"
                ") WITHOUT ROWID, STRICT;")
    );

    return ok();
}

cppbase::u64 M1674836144_genesis::get_version() const {
    return 1674836144;
}

}
