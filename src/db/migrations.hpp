#pragma once

#include <sqlite3.h>

#include <vector>

#include <util/structs/result.hpp>

class Migration {
public:
    static std::vector<std::unique_ptr<Migration>> get_migrations();

public:
    Migration() = default;
    virtual ~Migration() = default;

public:
    [[nodiscard]] virtual Result<void, int> up(sqlite3 * db) = 0;
    [[nodiscard]] virtual u64 get_version() const = 0;
};
