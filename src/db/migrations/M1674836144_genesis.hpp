#pragma once

#include <db/database.hpp>
#include <db/migrations.hpp>

class M1674836144_genesis final : public Migration {
public:
    M1674836144_genesis() = default;

public:
    Result<void, int> up(db::Database& db) override;
    [[nodiscard]] u64 get_version() const override;
};
