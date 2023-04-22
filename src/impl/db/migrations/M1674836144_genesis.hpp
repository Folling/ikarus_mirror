#pragma once

#include <cppbase/types.hpp>
#include <sqlitecpp/database.hpp>
#include <sqlitecpp/migrations.hpp>

namespace db {

class M1674836144_genesis final : public sqlitecpp::Migration {
public:
    M1674836144_genesis() = default;
    ~M1674836144_genesis() final = default;

public:
    [[nodiscard]] cppbase::Result<void, int> up(sqlitecpp::Database& db) override;
    [[nodiscard]] cppbase::u64 get_version() const override;
};

}
