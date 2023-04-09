#pragma once

#include "db/database.hpp"
#include "db/migrations.hpp"
#include "ikarus/id.h"

namespace db {

class M1674836160_initial_setup final : public Migration {
public:
    M1674836160_initial_setup() = default;
    ~M1674836160_initial_setup() = default;

public:
    [[nodiscard]] Result<void, int> up(db::Database& db) override;
    [[nodiscard]] u64 get_version() const override;
};

}
