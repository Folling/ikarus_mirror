#pragma once

#include "../id.h"
#include "../project.h"
#include "../status.h"

#include <cstdbool>
#include <cstddef>
#include <cstdint>

enum IkarusTemplateCreateV0Flags {
    IkarusTemplateCreateV0Flags_None = 0,
};

struct IkarusTemplateCreateV0Result {
    Id template;
    StatusCode status_code;
};

IkarusTemplateCreateV0Result ikarus_template_create_v0(
    Project * project, Id parent_folder, size_t position, char const * name, IkarusTemplateCreateV0Flags flags
);

enum IkarusTemplateDeleteV0Flags {
    IkarusTemplateDeleteV0Flags_None = 0,
};

struct IkarusTemplateDeleteV0Result {
    StatusCode status_code;
};

IkarusTemplateDeleteV0Result ikarus_template_delete_v0(Project * project, Id template, IkarusTemplateDeleteV0Flags flags);

enum IkarusTemplateGetpropertiesV0Flags {
    IkarusTemplateGetpropertiesV0Flags_None = 0,
};

struct IkarusTemplateGetpropertiesV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusTemplateGetpropertiesV0Result ikarus_template_get_properties_v0(
    Project * project, Id template, Id * properties_out, size_t properties_out_size, IkarusTemplateGetpropertiesV0Flags flags
);

enum IkarusTemplateGetpropertiesCountV0Flags {
    IkarusTemplateGetpropertiesCountV0Flags_None = 0,
};

struct IkarusTemplateGetpropertiesCountV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusTemplateGetpropertiesCountV0Result ikarus_template_get_properties_count_v0(
    Project * project, Id template, IkarusTemplateGetpropertiesCountV0Flags flags
);

enum IkarusTemplateGetpagesV0Flags {
    IkarusTemplateGetpagesV0Flags_None = 0,
};

struct IkarusTemplateGetpagesV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusTemplateGetpagesV0Result ikarus_template_get_pages_v0(
    Project * project, Id template, Id * pages_out, size_t pages_out_size, IkarusTemplateGetpagesV0Flags flags
);

enum IkarusTemplateGetpagesCountV0Flags {
    IkarusTemplateGetpagesCountV0Flags_None = 0,
};

struct IkarusTemplateGetpagesCountV0Result {
    size_t count;
    StatusCode status_code;
};

IkarusTemplateGetpagesCountV0Result ikarus_template_get_pages_count_v0(
    Project * project, Id template, IkarusTemplateGetpagesCountV0Flags flags);

