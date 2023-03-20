#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"


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


enum IkarusEntityGetInformationV0Flags {
     IkarusEntityGetInformationV0Flags_None = 0,
};

struct IkarusEntityGetInformationV0Result {
    char* information;
    StatusCode status_code;
};

IkarusEntityGetInformationV0Result ikarus_entity_get_information_v0(Project * project, Id entity, IkarusEntityGetInformationV0Flags flags);


enum IkarusEntitySetInformationV0Flags {
     IkarusEntitySetInformationV0Flags_None = 0,
};

struct IkarusEntitySetInformationV0Result {

    StatusCode status_code;
};

IkarusEntitySetInformationV0Result ikarus_entity_set_information_v0(Project * project, Id entity, char const* new_information, IkarusEntitySetInformationV0Flags flags);


enum IkarusEntityGetLocationV0Flags {
     IkarusEntityGetLocationV0Flags_None = 0,
};

struct IkarusEntityGetLocationV0Result {
    Id parent_folder;;
    size_t position;
    StatusCode status_code;
};

IkarusEntityGetLocationV0Result ikarus_entity_get_location_v0(Project * project, Id entity, IkarusEntityGetLocationV0Flags flags);


enum IkarusEntitySetLocationV0Flags {
     IkarusEntitySetLocationV0Flags_None = 0,
    IkarusEntitySetLocationV0Flags_IgnoreParent = 1 << 0};

struct IkarusEntitySetLocationV0Result {

    StatusCode status_code;
};

IkarusEntitySetLocationV0Result ikarus_entity_set_location_v0(Project * project, Id entity, Id new_parent, size_t new_position, IkarusEntitySetLocationV0Flags flags);

