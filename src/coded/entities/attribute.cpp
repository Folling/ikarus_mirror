#include "ikarus/entities/property.h"
#include "util/logger.hpp"
#include "validation/arg.hpp"

IkarusPropertyCreateResult ikarus_property_create_v1(
    Project * project,
    Id parent_folder,
    size_t position,
    Id template,
    char const * name,
    PropertyType type,
    char const * default_value,
    IkarusPropertyCreateV1Flags flags
) {
    LOG_FUNCTION_INFO("creating property");
    IkarusPropertyCreateResult ret{.property = id_null(), .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret, validation::validate_entity<validation::Exists, EntityType_Folder>(db_handle, parent_folder, "parent folder", &ret.status_code)
    );
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );

    CHECK(ret, validation::validate_string<validation::NotNull | validation::NotEmpty>(name, "name", &ret.status_code));

    return ret;
}

IkarusPropertyDeleteResult ikarus_property_delete_v1(Project * project, Id property, IkarusPropertyDeleteV1Flags flags) {}