#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"

// An entity serves as an umbrella term for all types of
// objects. Currently, the following entity types exist:
// Folders, Templates, Properties, and Pages.
//
// Every entity has a name and an Id. Names must be non-blank
// and non-empty strings. Each entity might have a parent
// folder. If it's parent folder is `id_null` it is a
// root-entity, otherwise it is a direct child of its parent.
// Next to the parent folder itself, each entity has a position
// within its parent folder's child-hierarchy. Positions start
// at 0.

enum IkarusEntityGetNameV0Flags {
    IkarusEntityGetNameV0Flags_None = 0,
};

struct IkarusEntityGetNameV0Result {
    char* name;
    StatusCode status_code;
};

IkarusEntityGetNameV0Result ikarus_entity_get_name_v0(Project * project, Id entity, IkarusEntityGetNameV0Flags flags);

enum IkarusEntitySetNameV0Flags {
    IkarusEntitySetNameV0Flags_None = 0,
};

struct IkarusEntitySetNameV0Result {
    StatusCode status_code;
};

IkarusEntitySetNameV0Result ikarus_entity_set_name_v0(Project * project, Id entity, char const* new_name, IkarusEntitySetNameV0Flags flags);

enum IkarusEntityGetLocationV0Flags {
    IkarusEntityGetLocationV0Flags_None = 0,
};

struct IkarusEntityGetLocationV0Result {
    Id parent_folder;
    size_t position;
    StatusCode status_code;
};

IkarusEntityGetLocationV0Result ikarus_entity_get_location_v0(Project * project, Id entity, IkarusEntityGetLocationV0Flags flags);

enum IkarusEntitySetLocationV0Flags {
    IkarusEntitySetLocationV0Flags_None = 0,
};

struct IkarusEntitySetLocationV0Result {
    StatusCode status_code;
};

IkarusEntitySetLocationV0Result ikarus_entity_set_location_v0(Project * project, Id entity, Id new_parent_folder, size_t new_position, IkarusEntitySetLocationV0Flags flags);
