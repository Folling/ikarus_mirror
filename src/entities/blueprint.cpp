#include "ikarus/entities/blueprint.h"

#include <db/db.hpp>
#include <entities/util.hpp>
#include <ikarus/status.h>
#include <project.hpp>
#include <util/status.hpp>
#include <validation/arg.hpp>

IkarusBlueprintCreateResult ikarus_blueprint_create_v1(
    Project * project, Id parent_folder, uint64_t position, char const * name, IkarusBlueprintCreateV1Flags flags
) {
    LOG_FUNCTION_INFO("creating blueprint");
    IkarusBlueprintCreateResult ret{.blueprint = id_null(), .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret, validation::validate_entity<validation::Exists, EntityType_Folder>(project, parent_folder, "parent folder", &ret.status_code)
    );

    LOG_INFO("creating blueprint with name {}", name);

    Id id = id_generate(EntityType_Blueprint);

    LOG_VERBOSE("generated id {}", id);

    TRYRV(ret, db::transact<true>(project->db, &ret.status_code, [&](sqlite3 * db) -> Result<void, int> {
              TRY(util::create_entity(db, id, parent_folder, position, name, "", &ret.status_code));

              LOG_VERBOSE("creating blueprints entry");
              TRY(db::exec(db, &ret.status_code, "INSERT INTO `blueprints`(`entity_id`) VALUES(?)", id));

              return ok();
          }));

    return ret;
}

IkarusBlueprintDeleteResult ikarus_blueprint_delete_v1(Project * project, Id blueprint, IkarusBlueprintDeleteV1Flags flags) {
    LOG_FUNCTION_INFO("deleting blueprint");
    IkarusBlueprintDeleteResult ret{.status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Blueprint>(
            project, blueprint, "blueprint", &ret.status_code
        )
    );

    TRYRV(ret, db::exec(project->db, &ret.status_code, "DELETE FROM `entities` WHERE `id` = ?", blueprint));

    if (db::changes(project->db) != 1) {
        LOG_WARN("blueprint doesn't exist");
        RETURN_STATUS(ret, StatusCode_NotFound);
    }

    return ret;
}

IkarusBlueprintGetAttributesResult ikarus_blueprint_get_attributes_v1(
    Project * project, Id blueprint, Id * attributes_out, size_t attributes_out_size, IkarusBlueprintGetAttributesV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching blueprint attributes");

    IkarusBlueprintGetAttributesResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Blueprint>(
            project, blueprint, "blueprint", &ret.status_code
        )
    );
    CHECK(ret, validation::validate_pointer<validation::NotNull>(attributes_out, "attributes buffer", &ret.status_code));

    if (attributes_out_size == 0) {
        LOG_WARN("passed buffer size was 0");
        return ret;
    }

    VTRYRV(
        ret.count,
        ret,
        db::get_many_buffered<Id>(
            project->db,
            &ret.status_code,
            "SELECT `id` FROM `attributes` WHERE `blueprint` = ?",
            attributes_out,
            attributes_out_size,
            blueprint
        )
    );

    return ret;
}

IkarusBlueprintGetAttributesCountResult ikarus_blueprint_get_attributes_count_v1(
    Project * project, Id blueprint, IkarusBlueprintGetAttributesCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching blueprint attributes count");

    IkarusBlueprintGetAttributesCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Blueprint>(
            project, blueprint, "blueprint", &ret.status_code
        )
    );

    VTRYRV(
        ret.count,
        ret,
        db::get_one<size_t>(project->db, &ret.status_code, "SELECT COUNT(*) FROM `attributes` WHERE `blueprint_id` = ?", blueprint)
    );

    return ret;
}

IkarusBlueprintGetInstancesResult ikarus_blueprint_get_instances_v1(
    Project * project, Id blueprint, Id * instances_out, size_t instances_out_size, IkarusBlueprintGetInstancesV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching blueprint instances");

    IkarusBlueprintGetInstancesResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Blueprint>(
            project, blueprint, "blueprint", &ret.status_code
        )
    );

    CHECK(
        ret, validation::validate_pointer<validation::NotNull>(instances_out, "instances buffer", &ret.status_code)

    )

    if (instances_out_size == 0) {
        LOG_WARN("passed buffer size was 0");
        return ret;
    }

    VTRYRV(
        ret.count,
        ret,
        db::get_many_buffered<Id>(
            project->db,
            &ret.status_code,
            "SELECT `id` FROM `instances` WHERE `blueprint` = ?",
            instances_out,
            instances_out_size,
            blueprint
        )
    );

    return ret;
}

IkarusBlueprintGetInstancesCountResult ikarus_blueprint_get_instances_count_v1(
    Project * project, Id blueprint, IkarusBlueprintGetInstancesCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching blueprint instances count");

    IkarusBlueprintGetInstancesCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
    CHECK(
        ret,
        validation::validate_entity<validation::NotNull | validation::Exists, EntityType_Blueprint>(
            project, blueprint, "blueprint", &ret.status_code
        )
    );

    VTRYRV(
        ret.count,
        ret,
        db::get_one<size_t>(project->db, &ret.status_code, "SELECT COUNT(*) FROM `instances` WHERE `blueprint_id` = ?", blueprint)
    );

    return ret;
}
