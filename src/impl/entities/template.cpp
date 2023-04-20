#include "ikarus/entities/template.h"

#include <impl/entities/util.hpp>
#include <impl/project.hpp>
#include <impl/status.hpp>
#include <impl/validation/arg.hpp>
#include <sqlitecpp/database.hpp>

IkarusTemplateCreateResult ikarus_template_create_v1(
    Project * project, Id parent_folder, uint64_t position, char const * name, IkarusTemplateCreateV1Flags flags
) {
    LOG_FUNCTION_INFO("creating template");
    IkarusTemplateCreateResult ret{.template = id_null(), .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret, validation::validate_entity<validation::Exists, EntityType_Folder>(db_handle, parent_folder, "parent folder", &ret.status_code)
    );
    CHECK(ret, validation::validate_string<validation::NotNull | validation::NotEmpty>(name, "name", &ret.status_code));

    LOG_INFO("creating template with name {}", name);

    Id id = id_generate(EntityType_Template);

    LOG_VERBOSE("generated id {}", id);

    TRYRV(ret, db_handle.get_db()->transact<false>(&ret.status_code, [&](auto& _) -> Result<void, int> {
        TRY(util::create_entity(db_handle, id, none(), parent_folder, position, name, "", &ret.status_code));

        LOG_VERBOSE("creating templates entry");
        TRY(db_handle.get_db()->exec(&ret.status_code, "INSERT INTO `templates`(`entity_id`) VALUES(?)", id));

        return ok();
    }));

    LOG_FUNCTION_SUCCESS("successfully created template");

    return ret;
}

IkarusTemplateDeleteResult ikarus_template_delete_v1(Project * project, Id template, IkarusTemplateDeleteV1Flags flags) {
    LOG_FUNCTION_INFO("deleting template");
    IkarusTemplateDeleteResult ret{.status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );

    TRYRV(ret, db_handle.get_db()->exec(&ret.status_code, "DELETE FROM `entities` WHERE `id` = ?", template));

    if (db_handle.get_db()->get_changes() != 1) {
        LOG_WARN("template doesn't exist");
        RETURN_STATUS(ret, StatusCode_NotFound);
    }

    LOG_FUNCTION_SUCCESS("successfully deleted template");

    return ret;
}

IkarusTemplateGetpropertiesResult ikarus_template_get_properties_v1(
    Project * project, Id template, Id * properties_out, size_t properties_out_size, IkarusTemplateGetpropertiesV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching template properties");

    IkarusTemplateGetpropertiesResult ret{.count = 0, .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );
    CHECK(ret, validation::validate_pointer<validation::NotNull>(properties_out, "properties buffer", &ret.status_code));

    if (properties_out_size == 0) {
        LOG_WARN("passed buffer size was 0");
        return ret;
    }

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_many_buffered<Id>(
            &ret.status_code, "SELECT `id` FROM `properties` WHERE `template` = ?", properties_out, properties_out_size, template
        )
    );

    LOG_FUNCTION_SUCCESS("successfully fetched properties");

    return ret;
}

IkarusTemplateGetpropertiesCountResult ikarus_template_get_properties_count_v1(
    Project * project, Id template, IkarusTemplateGetpropertiesCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching template properties count");

    IkarusTemplateGetpropertiesCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_one<size_t>(&ret.status_code, "SELECT COUNT(*) FROM `properties` WHERE `template_id` = ?", template)
    );

    LOG_FUNCTION_SUCCESS("successfully fetched properties count");

    return ret;
}

IkarusTemplateGetpagesResult ikarus_template_get_pages_v1(
    Project * project, Id template, Id * pages_out, size_t pages_out_size, IkarusTemplateGetpagesV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching template pages");

    IkarusTemplateGetpagesResult ret{.count = 0, .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );

    CHECK(ret, validation::validate_pointer<validation::NotNull>(pages_out, "pages buffer", &ret.status_code))

    if (pages_out_size == 0) {
        LOG_WARN("passed buffer size was 0");
        return ret;
    }

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_many_buffered<Id>(
            &ret.status_code, "SELECT `id` FROM `pages` WHERE `template` = ?", pages_out, pages_out_size, template
        )
    );

    LOG_FUNCTION_SUCCESS("successfully fetched pages");

    return ret;
}

IkarusTemplateGetpagesCountResult ikarus_template_get_pages_count_v1(
    Project * project, Id template, IkarusTemplateGetpagesCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching template pages count");

    IkarusTemplateGetpagesCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    auto db_handle = project->get_db_handle();

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Template>(
            db_handle, template, "template", &ret.status_code
        )
    );

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_one<size_t>(&ret.status_code, "SELECT COUNT(*) FROM `pages` WHERE `template_id` = ?", template)
    );

    LOG_FUNCTION_SUCCESS("successfully fetched pages count");

    return ret;
}
