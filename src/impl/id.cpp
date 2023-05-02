#include "ikarus/types/id.h"

#include <ikarus/types/entity_type.h>

#include <impl/project.hpp>

Id id_generate(Project * project, EntityType entity_type) {
    Id ret{0};

    ret.value |= (static_cast<cppbase::u64>(entity_type) << 56);
    ret.value |= (static_cast<cppbase::u64>(project->get_connection_id()) << 40);
    ret.value |= project->get_id_counter()++;

    return ret;
}

EntityType id_get_entity_type(Id id) {
    return static_cast<EntityType>(id.value >> 56);
}

bool id_is_none(Id id) {
    return id.value == ID_NONE.value;
}

bool id_is_unspecified(Id id) {
    return id.value == ID_UNSPECIFIED.value;
}
