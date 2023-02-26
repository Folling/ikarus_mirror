#include "ikarus/entities/entity.h"

#include "ikarus/entities/attributes/attribute.h"

#include <db/db.hpp>
#include <project.hpp>
#include <util/logger.hpp>
#include <util/status.hpp>
#include <validation/arg.hpp>

char const * ikarus_entity_get_name_v1(Project * project, Id entity, IkarusEntityGetNameV1Flags flags, StatusCode * status_out) {
    LOG_FUNCTION_VERBOSE("fetching entity name");
    VALIDATE_DB_ARG(Project, project, nullptr, nullptr);
    VALIDATE_DB_ARG(Entity, entity, nullptr, project->db);

    VTRYRVS(
        name,
        nullptr,
        StatusCode_InvalidArgument,
        db::get_one<db::RawString>(project->db, "SELECT `name` FROM `entities` WHERE `id` = ?", entity)
    );

    if (name.data == nullptr) {
        LOG_ERROR("database name was null");
        RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
    }

    char * ret = new char[name.size];

    std::strcpy(ret, name.data);

    return ret;
}

bool ikarus_entity_set_name_v1(
    Project * project, Id entity, char const * new_name, IkarusEntitySetNameV1Flags flags, StatusCode * status_out
) {
    LOG_FUNCTION_VERBOSE("fetching entity name");
    VALIDATE_ARG(Project, project, false);
    VALIDATE_DB_ARG(Entity, entity, false, project->db);
    VALIDATE_ARG(String, new_name, false);

    TRYRVS(false, StatusCode_InternalError, db::exec(project->db, "UPDATE `entities` SET `name` = ? WHERE `id` = ?", new_name, entity));

    return true;
}

char const * ikarus_entity_get_information_v1(
    Project * project, Id entity, IkarusEntityGetInformationV1Flags flags, StatusCode * status_out
);

bool ikarus_entity_set_information_v1(
    Project * project, Id entity, char const * new_information, IkarusEntitySetInformationV1Flags flags, StatusCode * status_out
) {
    LOG_FUNCTION_VERBOSE("updating entity information");
    VALIDATE_ARG(Project, project, false);
    VALIDATE_DB_ARG(Entity, entity, false, project->db);
    VALIDATE_ARG(String, new_information, false);

    TRYRVS(
        false,
        StatusCode_InternalError,
        db::exec(project->db, "UPDATE `entities` SET `information` = ? WHERE `id` = ?", new_information, entity)
    );

    return true;
}

Location ikarus_entity_get_location_v1(Project * project, Id entity, IkarusEntityGetLocationV1Flags flags, StatusCode * status_out);

bool ikarus_entity_set_location_v1(
    Project * project, Id entity, Id new_parent, size_t new_position, IkarusEntitySetLocationV1Flags flags, StatusCode * status_out
);

Id ikarus_entity_get_parent_v1(Project * project, Id entity, IkarusEntityGetParentV1Flags flags, StatusCode * status_out) {
    LOG_FUNCTION_VERBOSE("fetching entity parent");
    VALIDATE_DB_ARG(Project, project, id_null(), nullptr);
    VALIDATE_DB_ARG(Entity, entity, id_null(), project->db);

    IRTRYRVS(
        id_null(),
        StatusCode_InternalError,
        db::get_one<Id>(project->db, "SELECT IFNULL(`parent_id`, 0) FROM `tree` WHERE `entity_id` = ?", entity)
    );
}

bool ikarus_entity_set_parent_v1(
    Project * project, Id entity, Id new_parent, size_t new_position, IkarusEntitySetParentV1Flags flags, StatusCode * status_out
) {
    LOG_FUNCTION_INFO("updating entity parent");
    VALIDATE_ARG(Project, project, false);
    VALIDATE_NULLABLE_ARG(Entity, entity, false, project->db);

    LOG_VERBOSE("setting parent to {} at index {}", new_parent, new_position);

    std::optional<Id> tree_scope{};

    if (id_get_entity_type(entity) == EntityType_Attribute) {
        EVTRYRVS(
            tree_scope,
            false,
            StatusCode_InternalError,
            ikarus_attribute_get_blueprint_v1(project, entity, IkarusAttributeGetBlueprintV1Flags_None, status_out)
        );
    }

    TRYRVS(false, StatusCode_InternalError, db::transact<true>(project->db, [&](sqlite3 * db) -> Result<void, int> {
               LOG_VERBOSE("updating the positions of entities on the original parent where they would be after the entity is moved");

               TRY(db::exec(
                   project->db,
                   "UPDATE `tree` SET `position` = `position` - 1 WHERE "
                   "`parent_id` = (SELECT `parent_id` FROM `tree` WHERE `entity_id` = ?) AND "
                   "`position` > (SELECT `position` FROM `tree` WHERE `entity_id` = ?) AND "
                   "`scope` = ?",
                   entity,
                   tree_scope
               ));

               LOG_VERBOSE("updating the positions of entities on the new parent where they would be after the entity is moved");

               TRY(db::exec(
                   project->db,
                   "UPDATE `tree` SET `position` = `position` + 1 WHERE "
                   "`parent_id` = ? AND "
                   "`position` > ? AND "
                   "`scope` = ?",
                   new_parent,
                   new_position,
                   tree_scope
               ));

               LOG_VERBOSE("updating parent and position of the entity");

               TRY(db::exec(
                   project->db,
                   "UPDATE `tree` SET `parent_id` = ? AND `position` = ? WHERE `entity_id` = ?",
                   new_parent,
                   new_position,
                   entity
               ));

               return ok();
           }));

    return true;
}

size_t ikarus_entity_get_position_v1(Project * project, Id entity, IkarusEntityGetPositionV1Flags flags, StatusCode * status_out) {
    LOG_FUNCTION_VERBOSE("fetching entity position");
    VALIDATE_DB_ARG(Project, project, id_null(), nullptr);
    VALIDATE_DB_ARG(Entity, entity, id_null(), project->db);

    IRTRYRVS(
        id_null(),
        StatusCode_InternalError,
        db::get_one<Id>(project->db, "SELECT IFNULL(`parent_id`, 0) FROM `tree` WHERE `entity_id` = ?", entity)
    );
}
