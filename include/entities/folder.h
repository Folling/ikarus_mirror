#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../project.h"
#include "../error.h"
#include "../id.h"

struct Folder;

enum IkarusFolderCreateV1Flags {};

Id ikarus_folder_create_v1(Project * project, Id parent_folder, char const * name, IkarusFolderCreateV1Flags flags, ErrorCode * err_out);

enum IkarusFolderDeleteV1Flags {
    IkarusFolderDeleteV1Flags_KeepChildren = 1 << 0
};

bool ikarus_folder_delete_v1(Project * project, Id folder, IkarusFolderDeleteV1Flags flags, ErrorCode * err_out);

enum IkarusFolderGetChildrenV1Flags {
    IkarusFolderGetChildrenV1Flags_Recurse = 1 << 0
};

bool ikarus_folder_get_children_v1(Project * project, Id folder, Id* attributes_out, size_t attributes_out_size, IkarusFolderGetChildrenV1Flags flags, ErrorCode * err_out);

