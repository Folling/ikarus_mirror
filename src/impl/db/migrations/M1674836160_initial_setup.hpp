#pragma once

#include <cppbase/types.hpp>

#include <sqlitecpp/database.hpp>
#include <sqlitecpp/migrations.hpp>

namespace db {

class M1674836160_initial_setup final : public sqlitecpp::Migration {
public:
    M1674836160_initial_setup() = default;
    ~M1674836160_initial_setup() final = default;

public:
    [[nodiscard]] cppbase::Result<void, int> up(sqlitecpp::Database * db) override;
    [[nodiscard]] cppbase::u64 get_version() const override;
};

}
