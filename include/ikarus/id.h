#pragma once

#include <stdint.h>

typedef uint64_t Id;

enum EntityType { EntityType_None = 0, EntityType_Folder = 1, EntityType_Blueprint = 2, EntityType_Attribute = 3, EntityType_Instance = 4 };

// layout:
// first 8 bits: entity-type // 255 possible values, 0 for "nil/unknown"
// next 32 bits: seconds since the linux epoch (enough to last until 2106)
// next 24 bits: counter for all entities created during the same time interval

Id id_null();

Id id_generate(EntityType entity_type);

EntityType id_get_entity_type(Id id);

uint32_t id_get_timestamp(Id id);

uint32_t id_get_counter(Id id);

bool id_is_null(Id id);
