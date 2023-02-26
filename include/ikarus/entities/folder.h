#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"

enum IkarusFolderCreateV1Flags {
    IkarusFolderCreateV1Flags_None = 0,
};

struct IkarusFolderCreateResult {
    Id folder;
    StatusCode status_code;
};

IkarusFolderCreateResult ikarus_folder_create_v1(Project * project, Id parent_folder, char const * name, IkarusFolderCreateV1Flags flags);

enum IkarusFolderDeleteV1Flags {
    IkarusFolderDeleteV1Flags_None = 0,
    IkarusFolderDeleteV1Flags_KeepChildren = 1 << 0};

struct IkarusFolderDeleteResult {
    StatusCode status_code;
};

IkarusFolderDeleteResult ikarus_folder_delete_v1(Project * project, Id folder, IkarusFolderDeleteV1Flags flags);

enum IkarusFolderGetChildrenV1Flags {
    IkarusFolderGetChildrenV1Flags_None = 0,
    IkarusFolderGetChildrenV1Flags_Recurse = 1 << 0};

struct IkarusFolderGetChildrenResult {
    size_t count;
    StatusCode status_code;
};

IkarusFolderGetChildrenResult ikarus_folder_get_children_v1(Project * project, Id folder, Id* children_out, size_t children_out_size, IkarusFolderGetChildrenV1Flags flags);

