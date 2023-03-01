#pragma once

#include <sqlite3.h>

#include <string_view>

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

}