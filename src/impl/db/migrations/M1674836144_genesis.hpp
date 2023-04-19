#pragma once

#include <impl/db/database.hpp>
#include <impl/db/migrations.hpp>

namespace db {

class M1674836144_genesis final : public Migration {
public:
    M1674836144_genesis() = default;
    ~M1674836144_genesis() final = default;

public:
    [[nodiscard]] Result<void, int> up(db::Database& db) override;
    [[nodiscard]] u64 get_version() const override;
};

}
