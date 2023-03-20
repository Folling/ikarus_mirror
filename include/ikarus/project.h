#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "status.h"
#include "id.h"

struct Project;

enum IkarusProjectCreateV0Flags {
     IkarusProjectCreateV0Flags_None = 0,
};

struct IkarusProjectCreateV0Result {
    Project * project;
    StatusCode status_code;
};

IkarusProjectCreateV0Result ikarus_project_create_v0(char const * path, IkarusProjectCreateV0Flags flags);


enum IkarusProjectOpenV0Flags {
     IkarusProjectOpenV0Flags_None = 0,
};

struct IkarusProjectOpenV0Result {
    Project * project;
    StatusCode status_code;
};

IkarusProjectOpenV0Result ikarus_project_open_v0(char const* path, IkarusProjectOpenV0Flags flags);


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
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetEntitiesV0Result ikarus_project_get_entities_v0(Project const * project, Id* entities_out, size_t entities_out_size, size_t entity_types, IkarusProjectGetEntitiesV0Flags flags);


enum IkarusProjectGetEntitiesCountV0Flags {
     IkarusProjectGetEntitiesCountV0Flags_None = 0,
};

struct IkarusProjectGetEntitiesCountV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetEntitiesCountV0Result ikarus_project_get_entities_count_v0(Project const * project, size_t entity_types, IkarusProjectGetEntitiesCountV0Flags flags);

