#pragma once

#include "db/db.hpp"
#include "db/migrations.hpp"

class M1674836144_genesis final : public Migration {
public:
    M1674836144_genesis() = default;

public:
    inline Result<void, int> up(sqlite3 * db) override {
        LOG_VERBOSE("creating metadata table");

        TRY(db::exec(
            db,
            nullptr,
            "CREATE TABLE `metadata`(\n"
            // INT is important here - INTEGER would make it an alias for the rowid
            "    `key` TEXT PRIMARY KEY,\n"
            "    `value` TEXT\n"
            ") WITHOUT ROWID, STRICT;"
        ));

        return ok();
    }

    [[nodiscard]] inline u64 get_version() const override {
        return 1674836144;
    }
};
