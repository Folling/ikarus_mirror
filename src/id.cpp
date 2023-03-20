#include "id.h"

#include <atomic>
#include <chrono>
#include <thread>

#include <util/base.hpp>

std::atomic<u32> counter{0};
std::atomic<u64> last_second;
std::chrono::duration waiting_period{std::chrono::milliseconds{1}};

Id id_null() {
    return 0;
}

Id id_generate(EntityType entity_type) {
    Id ret;
    ret |= (static_cast<u64>(entity_type) << 56);

    auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::high_resolution_clock::now());
    auto second = time.time_since_epoch().count();

    // limit defined by amount of bits as defined in id.h, 2^bits are available for the counter
    if (counter >= 16777216) {
        LOG_TRACE("counter hit limit, waiting until next second in {} increments", waiting_period);

        auto new_second = second;

        while (new_second == second) {
            time = std::chrono::floor<std::chrono::seconds>(std::chrono::high_resolution_clock::now());
            new_second = time.time_since_epoch().count();
            std::this_thread::sleep_for(waiting_period);
        }

        LOG_TRACE("done waiting");

        second = new_second;
    }

    if (last_second != second) {
        counter = 0;
    }

    ret |= (second << 24);

    ret |= (counter++ & 0x00FFFFFF);

    last_second = second;

    return ret;
}

EntityType id_get_entity_type(Id id) {
    return static_cast<EntityType>(id >> 56);
}

uint32_t id_get_timestamp(Id id) {
    return static_cast<uint32_t>(id >> 24);
}

uint32_t id_get_counter(Id id) {
    return static_cast<uint32_t>(id & 0x00FFFFFF);
}

bool id_is_null(Id id) {
    return id == 0;
}
