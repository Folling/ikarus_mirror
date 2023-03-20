#pragma once

#include <sqlite3.h>

#include <string_view>

#include <db/sqlitehelper.hpp>
#include <db/util.hpp>
#include <status.hpp>
#include <util/structs/result.hpp>

namespace db {

class Statement {
public:
    friend class Database;

public:
    Statement();
    Statement(Statement& other) = delete;
    Statement(Statement&& other) noexcept;
    ~Statement();

    Statement& operator=(Statement& other) = delete;
    Statement& operator=(Statement&& other) noexcept;

public:
    template<typename... Args>
    [[nodiscard]] Result<void, int> prepare(sqlite3 * db, StatusCode * status_out, std::string_view stmt, Args&&... args);

private:
    sqlite3_stmt * _handle;
};

template<typename... Args>
Result<void, int> Statement::prepare(sqlite3 * db, StatusCode * status_out, std::string_view stmt, Args&&... args) {
    if (auto rc = sqlite3_prepare_v3(db, stmt.data(), static_cast<int>(stmt.size()), 0, &_handle, nullptr); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to prepare statement", db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }

    if (auto rc = detail::bind(_handle, std::forward<Args>(args)...); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to bind parameter", db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }

    return ok();
}

}