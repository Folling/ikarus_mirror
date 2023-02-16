#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "id.h"

struct Project;

enum IkarusProjectCreateV1Flags {};

Project * ikarus_project_create_v1(char const * path, IkarusProjectCreateV1Flags flags, ErrorCode * err_out);

enum IkarusProjectOpenV1Flags {};

Project * ikarus_project_open_v1(char const* path, IkarusProjectOpenV1Flags flags, ErrorCode * err_out);

enum IkarusProjectCloseV1Flags {};

bool ikarus_project_close_v1(Project * project, IkarusProjectCloseV1Flags flags, ErrorCode * err_out);

enum IkarusProjectDeleteV1Flags {};

bool ikarus_project_delete_v1(Project * project, IkarusProjectDeleteV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetRootEntitiesV1Flags {};

bool ikarus_project_get_root_entities_v1(Project const * project, Id* entities_out, size_t entities_out_size, IkarusProjectGetRootEntitiesV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetTreeV1Flags {};

bool ikarus_project_get_tree_v1(Project const * project, Id* entity_ids_out, Id* entity_parent_ids_out, size_t* entity_positions_out, char const** entity_names_out, size_t entities_out_size, IkarusProjectGetTreeV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetRootEntitiesCountV1Flags {};

size_t ikarus_project_get_root_entities_count_v1(Project const * project, IkarusProjectGetRootEntitiesCountV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetBlueprintsV1Flags {};

bool ikarus_project_get_blueprints_v1(Project const * project, Id* blueprints_out, size_t blueprints_out_size, IkarusProjectGetBlueprintsV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetBlueprintsCountV1Flags {};

size_t ikarus_project_get_blueprints_count_v1(Project const * project, IkarusProjectGetBlueprintsCountV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetAttributesV1Flags {};

bool ikarus_project_get_attributes_v1(Project const * project, Id* attributes_out, size_t attributes_out_out_size, IkarusProjectGetAttributesV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetAttributesCountV1Flags {};

size_t ikarus_project_get_attributes_count_v1(Project const * project, IkarusProjectGetAttributesCountV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetInstancesV1Flags {};

bool ikarus_project_get_instances_v1(Project const * project, Id* instances_out, size_t instances_out_size, IkarusProjectGetInstancesV1Flags flags, ErrorCode * err_out);

enum IkarusProjectGetInstancesCountV1Flags {};

size_t ikarus_project_get_instances_count_v1(Project const * project, IkarusProjectGetInstancesCountV1Flags flags, ErrorCode * err_out);

