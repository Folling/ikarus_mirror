#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "status.h"
#include "id.h"

struct Project;

enum IkarusProjectCreateV1Flags {
    IkarusProjectCreateV1Flags_None = 0,
};

struct IkarusProjectCreateResult {
    Project * project;
    StatusCode status_code;
};

IkarusProjectCreateResult ikarus_project_create_v1(char const * path, IkarusProjectCreateV1Flags flags);

enum IkarusProjectOpenV1Flags {
    IkarusProjectOpenV1Flags_None = 0,
};

struct IkarusProjectOpenResult {
    Project * project;
    StatusCode status_code;
};

IkarusProjectOpenResult ikarus_project_open_v1(char const* path, IkarusProjectOpenV1Flags flags);

enum IkarusProjectCloseV1Flags {
    IkarusProjectCloseV1Flags_None = 0,
};

struct IkarusProjectCloseResult {
    StatusCode status_code;
};

IkarusProjectCloseResult ikarus_project_close_v1(Project * project, IkarusProjectCloseV1Flags flags);

enum IkarusProjectDeleteV1Flags {
    IkarusProjectDeleteV1Flags_None = 0,
};

struct IkarusProjectDeleteResult {
    StatusCode status_code;
};

IkarusProjectDeleteResult ikarus_project_delete_v1(Project * project, IkarusProjectDeleteV1Flags flags);

enum IkarusProjectGetRootEntitiesV1Flags {
    IkarusProjectGetRootEntitiesV1Flags_None = 0,
};

struct IkarusProjectGetRootEntitiesResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetRootEntitiesResult ikarus_project_get_root_entities_v1(Project const * project, Id* entities_out, size_t entities_out_size, IkarusProjectGetRootEntitiesV1Flags flags);

enum IkarusProjectGetRootEntitiesCountV1Flags {
    IkarusProjectGetRootEntitiesCountV1Flags_None = 0,
};

struct IkarusProjectGetRootEntitiesCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetRootEntitiesCountResult ikarus_project_get_root_entities_count_v1(Project const * project, IkarusProjectGetRootEntitiesCountV1Flags flags);

enum IkarusProjectGetBlueprintsV1Flags {
    IkarusProjectGetBlueprintsV1Flags_None = 0,
};

struct IkarusProjectGetBlueprintsResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetBlueprintsResult ikarus_project_get_blueprints_v1(Project const * project, Id* blueprints_out, size_t blueprints_out_size, IkarusProjectGetBlueprintsV1Flags flags);

enum IkarusProjectGetBlueprintsCountV1Flags {
    IkarusProjectGetBlueprintsCountV1Flags_None = 0,
};

struct IkarusProjectGetBlueprintsCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetBlueprintsCountResult ikarus_project_get_blueprints_count_v1(Project const * project, IkarusProjectGetBlueprintsCountV1Flags flags);

enum IkarusProjectGetAttributesV1Flags {
    IkarusProjectGetAttributesV1Flags_None = 0,
};

struct IkarusProjectGetAttributesResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetAttributesResult ikarus_project_get_attributes_v1(Project const * project, Id* attributes_out, size_t attributes_out_out_size, IkarusProjectGetAttributesV1Flags flags);

enum IkarusProjectGetAttributesCountV1Flags {
    IkarusProjectGetAttributesCountV1Flags_None = 0,
};

struct IkarusProjectGetAttributesCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetAttributesCountResult ikarus_project_get_attributes_count_v1(Project const * project, IkarusProjectGetAttributesCountV1Flags flags);

enum IkarusProjectGetInstancesV1Flags {
    IkarusProjectGetInstancesV1Flags_None = 0,
};

struct IkarusProjectGetInstancesResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetInstancesResult ikarus_project_get_instances_v1(Project const * project, Id* instances_out, size_t instances_out_size, IkarusProjectGetInstancesV1Flags flags);

enum IkarusProjectGetInstancesCountV1Flags {
    IkarusProjectGetInstancesCountV1Flags_None = 0,
};

struct IkarusProjectGetInstancesCountResult {
    size_t count;
    StatusCode status_code;
};

IkarusProjectGetInstancesCountResult ikarus_project_get_instances_count_v1(Project const * project, IkarusProjectGetInstancesCountV1Flags flags);

