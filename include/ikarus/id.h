#pragma once

#include "entity_type.h"

#include <stdint.h>

typedef uint64_t Id;

// TODO rewrite to UUID with injection of entity-type

// The difference between NONE and UNSPECIFIED is useful for situations where "null" has a meaning, but you still want optional parameters
// Take the parent-folder field of an entity for example.
// Using `set_location` you can specify ID_NONE as the parent to make the item a root entity
// However using ID_UNSPECIFIED would mean that you don't want to specify the parent folder,
// which in that context means that you want to only update the position
// colloquially these are known as "null IDs"
const Id ID_NONE = 0;
const Id ID_UNSPECIFIED = 1;

class Project;

// layout:
// first 8 bits: entity-type // 255 possible values, 0 for "nil/unknown"
// next 16 bits: connection id (generated by the database) TODO
// next 40 bits: incremented counter for connection-id

Id id_generate(Project* project, EntityType entity_type);

EntityType id_get_entity_type(Id id);
