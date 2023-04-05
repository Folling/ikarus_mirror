#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "status.h"
#include "id.h"
#include "path.h"
#include "entity_type.h"

// A project represents a persistent workspace of Ikarus
// related data. Each project can contain any number of
// entities. The data is stored as a singular SQLite database.
struct Project;


enum IkarusProjectOpenV0Flags {
    IkarusProjectOpenV0Flags_None = 0,
    IkarusProjectOpenV0Flags_MustExist = 1 << 0,
    IkarusProjectOpenV0Flags_CreateParents = 1 << 1
};

struct IkarusProjectOpenV0Result {
    Project * project;
    StatusCode status_code;
};

IkarusProjectOpenV0Result ikarus_project_open_v0(Path path, IkarusProjectOpenV0Flags flags);

enum IkarusProjectCloseV0Flags {
    IkarusProjectCloseV0Flags_None = 0,
};

struct IkarusProjectCloseV0Result {
    StatusCode status_code;
};

IkarusProjectCloseV0Result ikarus_project_close_v0(Project * project, IkarusProjectCloseV0Flags flags);

enum IkarusProjectDeleteV0Flags {
    IkarusProjectDeleteV0Flags_None = 0,
};

struct IkarusProjectDeleteV0Result {
    StatusCode status_code;
};

IkarusProjectDeleteV0Result ikarus_project_delete_v0(Project * project, IkarusProjectDeleteV0Flags flags);

enum IkarusProjectGetEntitiesV0Flags {
    IkarusProjectGetEntitiesV0Flags_None = 0,
};

struct IkarusProjectGetEntitiesV0Result {
    StatusCode status_code;
};

IkarusProjectGetEntitiesV0Result ikarus_project_get_entities_v0(Project const * project, Id* entities_out, size_t entities_out_size, EntityTypes EntityTypes, IkarusProjectGetEntitiesV0Flags flags);

enum IkarusProjectGetEntitiesCountV0Flags {
    IkarusProjectGetEntitiesCountV0Flags_None = 0,
};

struct IkarusProjectGetEntitiesCountV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetEntitiesCountV0Result ikarus_project_get_entities_count_v0(Project const * project, EntityTypes entity_types, IkarusProjectGetEntitiesCountV0Flags flags);
