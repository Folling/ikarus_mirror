#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include "../project.h"
#include "../status.h"
#include "../id.h"


enum IkaruspageCreateV0Flags {
     IkaruspageCreateV0Flags_None = 0,
};

struct IkaruspageCreateV0Result {
    Id page;
    StatusCode status_code;
};

IkaruspageCreateV0Result ikarus_page_create_v0(Project * project, Id template, Id parent_folder, size_t position, char const * name, IkaruspageCreateV0Flags flags);


enum IkaruspageDeleteV0Flags {
     IkaruspageDeleteV0Flags_None = 0,
};

struct IkaruspageDeleteV0Result {

    StatusCode status_code;
};

IkaruspageDeleteV0Result ikarus_page_delete_v0(Project * project, Id page, IkaruspageDeleteV0Flags flags);


enum IkaruspageGetTemplateV0Flags {
     IkaruspageGetTemplateV0Flags_None = 0,
};

struct IkaruspageGetTemplateV0Result {
    Id template;
    StatusCode status_code;
};

IkaruspageGetTemplateV0Result ikarus_page_get_template_v0(Project* project, Id page, IkaruspageGetTemplateV0Flags flags);

