#include "M1674836144_genesis.hpp"

#include <cppbase/logger.hpp>
#include <sqlitecpp/database.hpp>

namespace db {

cppbase::Result<void, int> M1674836144_genesis::up(sqlitecpp::Database& db) {
    LOG_VERBOSE("creating metadata table");

    TRY(
        db.exec("CREATE TABLE `metadata`(\n"
                // INT is important here - INTEGER would make it an alias for the rowid
                "    `key` TEXT PRIMARY KEY,\n"
                "    `value` TEXT\n"
                ") WITHOUT ROWID, STRICT;")
    );

    return cppbase::ok();
}

cppbase::u64 M1674836144_genesis::get_version() const {
    return 1674836144;
}

}
