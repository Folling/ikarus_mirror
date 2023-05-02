#pragma once

#include "ikarus/types/id.h"

#include <cppbase/option.hpp>
#include <cppbase/types.hpp>

#include <sqlitecpp/sqlite_helper.hpp>

namespace sqlitecpp::detail {

template<>
struct SQLiteHelper<Id> {
    static Id convert(sqlite3_stmt * stmt, std::size_t idx) {
        auto value = SQLiteHelper<cppbase::Option<cppbase::u64>>::convert(stmt, idx);

        return Id{value.unwrap_or(0)};
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, Id const& id) {
        if (id_is_none(id)) {
            return sqlite3_bind_null(stmt, static_cast<int>(idx));
        } else if (id_is_unspecified(id)) {
            LOG_ERROR("attempted to bind ID_UNSPECIFIED to SQLite statement");
            return SQLITE_MISUSE;
        } else {
            return sqlite3_bind_int64(stmt, static_cast<int>(idx), static_cast<sqlite_int64>(id.value));
        }
    }
};

}
