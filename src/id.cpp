#include "ikarus/id.h"

#include <atomic>
#include <chrono>
#include <thread>

#include <util/base.hpp>

#include <project.hpp>

Id id_generate(Project* project, EntityType entity_type) {
    Id ret{0};

    ret |= (static_cast<u64>(entity_type) << 56);
    ret |= (static_cast<u64>(project->get_connection_id()) << 40);
    ret |= project->get_id_counter()++;

    return ret;
}

EntityType id_get_entity_type(Id id) {
    return static_cast<EntityType>(id >> 56);
}
