#pragma once

#include "util/structs/result.hpp"

#include <memory>
#include <vector>

namespace db {

class Database;

class Migration {
public:
    static std::vector<std::unique_ptr<Migration>> get_migrations();

public:
    Migration() = default;
    virtual ~Migration() = default;

public:
    [[nodiscard]] virtual Result<void, int> up(db::Database& db) = 0;
    [[nodiscard]] virtual u64 get_version() const = 0;
};

}
