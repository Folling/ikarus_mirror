#include "migrations.hpp"

#include "db/migrations/M1674836144_genesis.hpp"
#include "db/migrations/M1674836160_initial_setup.hpp"

namespace db {

std::vector<std::unique_ptr<Migration>> Migration::get_migrations() {
    std::vector<std::unique_ptr<Migration>> ret{};

    ret.emplace_back(std::make_unique<M1674836144_genesis>());
    ret.emplace_back(std::make_unique<M1674836160_initial_setup>());

    return ret;
}

}
