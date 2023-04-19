#pragma once

#include <sqlite3.h>

#include <string_view>

#include <impl/db/sqlitehelper.hpp>
#include <impl/db/util.hpp>
#include <impl/status.hpp>
#include <impl/util/structs/result.hpp>

namespace db {

class Statement {
public:
    friend class Database;

public:
    Statement(Statement& other) = delete;
    Statement(Statement&& other) noexcept;
    ~Statement();

    Statement& operator=(Statement& other) = delete;
    Statement& operator=(Statement&& other) noexcept;

private:
    explicit Statement(sqlite3_stmt * handle);

public:
    template<typename... Args>
    [[nodiscard]] static Result<Statement, int> prepare(sqlite3 * db, StatusCode * status_out, std::string_view stmt, Args&&... args);

private:
    sqlite3_stmt * _handle;
};

template<typename... Args>
Result<Statement, int> Statement::prepare(sqlite3 * db, StatusCode * status_out, std::string_view stmt, Args&&... args) {
    sqlite3_stmt * handle;

    if (auto rc = sqlite3_prepare_v3(db, stmt.data(), static_cast<int>(stmt.size()), 0, &handle, nullptr); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to prepare statement", db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }

    if (auto rc = detail::bind(handle, std::forward<Args>(args)...); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to bind parameter", db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }

    return ok(Statement{handle});
}

}