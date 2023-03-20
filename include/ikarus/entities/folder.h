#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"


enum IkarusFolderCreateV0Flags {
     IkarusFolderCreateV0Flags_None = 0,
};

struct IkarusFolderCreateV0Result {
    Id folder;
    StatusCode status_code;
};

IkarusFolderCreateV0Result ikarus_folder_create_v0(Project * project, Id parent_folder, char const * name, IkarusFolderCreateV0Flags flags);


enum IkarusFolderDeleteV0Flags {
     IkarusFolderDeleteV0Flags_None = 0,
    IkarusFolderDeleteV0Flags_KeepChildren = 1 << 0};

struct IkarusFolderDeleteV0Result {

    StatusCode status_code;
};

IkarusFolderDeleteV0Result ikarus_folder_delete_v0(Project * project, Id folder, IkarusFolderDeleteV0Flags flags);


enum IkarusFolderGetChildrenV0Flags {
     IkarusFolderGetChildrenV0Flags_None = 0,
    IkarusFolderGetChildrenV0Flags_Recurse = 1 << 0};

struct IkarusFolderGetChildrenV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusFolderGetChildrenV0Result ikarus_folder_get_children_v0(Project * project, Id folder, Id* children_out, size_t children_out_size, IkarusFolderGetChildrenV0Flags flags);

