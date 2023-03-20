#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"

enum IkarusEntityGetNameV1Flags {
    IkarusEntityGetNameV1Flags_None = 0,
};

struct IkarusEntityGetNameResult {
    char* name;
    StatusCode status_code;
};

IkarusEntityGetNameResult ikarus_entity_get_name_v1(Project * project, Id entity, IkarusEntityGetNameV1Flags flags);

enum IkarusEntitySetNameV1Flags {
    IkarusEntitySetNameV1Flags_None = 0,
};

struct IkarusEntitySetNameResult {
    StatusCode status_code;
};

IkarusEntitySetNameResult ikarus_entity_set_name_v1(Project * project, Id entity, char const* new_name, IkarusEntitySetNameV1Flags flags);

enum IkarusEntityGetInformationV1Flags {
    IkarusEntityGetInformationV1Flags_None = 0,
};

struct IkarusEntityGetInformationResult {
    char* information;
    StatusCode status_code;
};

IkarusEntityGetInformationResult ikarus_entity_get_information_v1(Project * project, Id entity, IkarusEntityGetInformationV1Flags flags);

enum IkarusEntitySetInformationV1Flags {
    IkarusEntitySetInformationV1Flags_None = 0,
};

struct IkarusEntitySetInformationResult {
    StatusCode status_code;
};

IkarusEntitySetInformationResult ikarus_entity_set_information_v1(Project * project, Id entity, char const* new_information, IkarusEntitySetInformationV1Flags flags);

enum IkarusEntityGetLocationV1Flags {
    IkarusEntityGetLocationV1Flags_None = 0,
};

struct IkarusEntityGetLocationResult {
    Id parent_folder;
    size_t position;
    StatusCode status_code;
};

IkarusEntityGetLocationResult ikarus_entity_get_location_v1(Project * project, Id entity, IkarusEntityGetLocationV1Flags flags);

enum IkarusEntitySetLocationV1Flags {
    IkarusEntitySetLocationV1Flags_None = 0,
    IkarusEntitySetLocationV1Flags_IgnoreParent = 1 << 0};

struct IkarusEntitySetLocationResult {
    StatusCode status_code;
};

IkarusEntitySetLocationResult ikarus_entity_set_location_v1(Project * project, Id entity, Id new_parent, size_t new_position, IkarusEntitySetLocationV1Flags flags);

