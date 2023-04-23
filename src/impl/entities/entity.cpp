// #include "ikarus/entities/entity.h"
//
// #include <cppbase/logger.hpp>
//
// #include <sqlitecpp/database.hpp>
//
// #include <impl/entities/util.hpp>
// #include <impl/project.hpp>
// #include <impl/status.hpp>
// #include <impl/validation/arg.hpp>
//
// IkarusEntityGetNameResult ikarus_entity_get_name_v1(Project * project, Id entity, IkarusEntityGetNameV1Flags flags) {
//     LOG_FUNCTION_VERBOSE("fetching entity name");
//
//     IkarusEntityGetNameResult ret{.name = nullptr, .status_code = StatusCode_Ok};
//
//     auto db_handle = project->get_db_handle();
//
//     ret.name = util::fetch_single_string(db_handle, entity, "entity", "name", "id", &ret.status_code);
//
//     LOG_FUNCTION_SUCCESS("successfully fetched name");
//
//     return ret;
// }
//
// IkarusEntitySetNameResult ikarus_entity_set_name_v1(Project * project, Id entity, char const * new_name, IkarusEntitySetNameV1Flags
// flags) {
//     LOG_FUNCTION_INFO("changing entity name");
//
//     IkarusEntitySetNameResult ret{.status_code = StatusCode_Ok};
//
//     auto db_handle = project->get_db_handle();
//
//     util::update_single_string<validation::ValidationFlags::NotEmpty>(
//         db_handle, entity, "entity", "name", "id", new_name, &ret.status_code
//     );
//
//     LOG_FUNCTION_SUCCESS("successfully changed name");
//
//     return ret;
// }
//
// IkarusEntityGetInformationResult ikarus_entity_get_information_v1(Project * project, Id entity, IkarusEntityGetInformationV1Flags flags)
// {
//     LOG_FUNCTION_VERBOSE("fetching entity information");
//
//     IkarusEntityGetInformationResult ret{.information = nullptr, .status_code = StatusCode_Ok};
//
//     auto db_handle = project->get_db_handle();
//
//     ret.information = util::fetch_single_string(db_handle, entity, "entity", "information", "id", &ret.status_code);
//
//     LOG_FUNCTION_SUCCESS("successfully fetched information");
//
//     return ret;
// }
//
// IkarusEntitySetInformationResult ikarus_entity_set_information_v1(
//     Project * project, Id entity, char const * new_information, IkarusEntitySetInformationV1Flags flags
//) {
//     LOG_FUNCTION_INFO("changing entity information");
//
//     IkarusEntitySetInformationResult ret{.status_code = StatusCode_Ok};
//
//     auto db_handle = project->get_db_handle();
//
//     util::update_single_string<validation::ValidationFlags::None>(
//         db_handle, entity, "entity", "information", "id", new_information, &ret.status_code
//     );
//
//     LOG_FUNCTION_SUCCESS("successfully changed information");
//
//     return ret;
// }
//
// IkarusEntityGetLocationResult ikarus_entity_get_location_v1(Project * project, Id entity, IkarusEntityGetLocationV1Flags flags) {
//     LOG_FUNCTION_VERBOSE("fetching entity location");
//
//     IkarusEntityGetLocationResult ret{.parent_folder = id_null(), .position = 0, .status_code = StatusCode_Ok};
//
//     CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
//
//     auto db_handle = project->get_db_handle();
//
//     CHECK(
//         ret,
//         validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(
//             db_handle, entity, "entity", &ret.status_code
//         )
//     );
//
//     VTRYRV(
//         auto tuple,
//         ret,
//         db_handle.get_db()->get_one<Id, size_t>(
//             &ret.status_code, "SELECT IFNULL(`parent_id`, 0), `position` FROM `entity_tree` WHERE `entity_id` = ?", entity
//         )
//     );
//
//     ret.parent_folder = std::get<0>(tuple);
//     ret.position = std::get<1>(tuple);
//
//     LOG_FUNCTION_SUCCESS("successfully fetched location");
//
//     return ret;
// }
//
// IkarusEntitySetLocationResult ikarus_entity_set_location_v1(
//     Project * project, Id entity, Id new_parent, size_t new_position, IkarusEntitySetLocationV1Flags flags
//) {
//     LOG_FUNCTION_INFO("changing entity location");
//
//     IkarusEntitySetLocationResult ret{.status_code = StatusCode_Ok};
//
//     CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));
//
//     auto db_handle = project->get_db_handle();
//
//     CHECK(
//         ret,
//         validation::validate_entity<validation::NotNull | validation::Exists, EntityType_None>(
//             db_handle, entity, "entity", &ret.status_code
//         )
//     );
//
//     CHECK(
//         ret, validation::validate_entity<validation::Exists, EntityType_Folder>(db_handle, new_parent, "parent folder", &ret.status_code)
//     );
//
//     LOG_VERBOSE("fetching current location");
//
//     IkarusEntityGetLocationResult location_res = ikarus_entity_get_location_v1(project, entity, IkarusEntityGetLocationV1Flags_None);
//
//     if (location_res.status_code != StatusCode_Ok) {
//         RETURN_STATUS(ret, location_res.status_code);
//     }
//
//     Id current_parent = location_res.parent_folder;
//     size_t current_position = location_res.position;
//
//     LOG_VERBOSE("fetching scope");
//
//     VTRYRV(
//         auto scope,
//         ret,
//         db_handle.get_db()->get_one<Option<Id>>(&ret.status_code, "SELECT `scope` FROM `entity_tree` WHERE `id` = ?", entity)
//     );
//
//     LOG_VERBOSE("fetching children count");
//
//     VTRYRV(
//         size_t children_count,
//         ret,
//         db_handle.get_db()->get_one<size_t>(
//             &ret.status_code,
//             "SELECT COUNT(*) FROM `entity_tree` WHERE "
//             "`scope` = ? AND "
//             "`parent_id` = ?",
//             scope,
//             util::new_parent
//         )
//     );
//
//     LOG_VERBOSE("validating new position");
//
//     bool valid_position;
//
//     if (current_parent == new_parent) {
//         valid_position = new_position < children_count;
//     } else {
//         valid_position = new_position <= children_count;
//     }
//
//     if (!valid_position) {
//         LOG_ERROR("position is out of bounds for parent");
//         RETURN_STATUS(ret, StatusCode_InvalidArgument);
//     }
//
//     LOG_VERBOSE("updating current siblings' positions");
//
//     TRYRV(
//         ret,
//         db_handle.get_db()->exec(
//             &ret.status_code,
//             "UPDATE `entity_tree` SET `position` = `position` - 1 WHERE "
//             "`scope` = ? AND "
//             "`parent` = ? AND "
//             "`position` > ?",
//             scope,
//             util::current_parent,
//             current_position
//         )
//     );
//
//     LOG_VERBOSE("updating new siblings' positions");
//
//     TRYRV(
//         ret,
//         db_handle.get_db()->exec(
//             &ret.status_code,
//             "UPDATE `entity_tree` SET `position` = `position` + 1 WHERE "
//             "`scope` = ? AND "
//             "`parent` = ? AND "
//             "`position` >= ?",
//             scope,
//             util::new_parent,
//             new_position
//         )
//     );
//
//     LOG_VERBOSE("updating location");
//
//     TRYRV(
//         ret,
//         db_handle.get_db()->exec(
//             &ret.status_code,
//             "UPDATE `entity_tree` SET `parent_id` = ?, `position` = ? WHERE "
//             "`entity_id` = ?",
//             util::new_parent,
//             new_position,
//             entity
//         )
//     );
//
//     LOG_FUNCTION_SUCCESS("successfully changed location");
//
//     return ret;
// }
