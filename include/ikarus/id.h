#pragma once

#include <stdint.h>

typedef uint64_t Id;

// The difference between NULL and UNSET is useful for situations where "null" has a meaning, but you still want optional parameters
// Take the parent-folder field of an entity for example.
// Using `set_location` you can specify ID_NULL as the parent to make the item a root entity
// However using ID_UNSET would mean that you don't want to specify the parent folder,
// which in that context means that you want to only update the position
const Id ID_NULL = 0;
const Id ID_UNSET = 1;

enum EntityType { EntityType_None = 0, EntityType_Folder = 1, EntityType_Template = 2, EntityType_Property = 3, EntityType_page = 4 };

// layout:
// first 8 bits: entity-type // 255 possible values, 0 for "nil/unknown"
// next 32 bits: seconds since the linux epoch (enough to last until 2106)
// next 24 bits: counter for all entities created during the same time interval

Id id_null();

Id id_generate(EntityType entity_type);

EntityType id_get_entity_type(Id id);

uint32_t id_get_timestamp(Id id);

uint32_t id_get_counter(Id id);
