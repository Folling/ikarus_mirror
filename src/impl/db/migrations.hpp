#pragma once

#include <memory>
#include <vector>

#include <impl/util/structs/result.hpp>
#include <impl/util/types.hpp>

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
