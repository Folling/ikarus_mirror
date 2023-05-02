#include "generated/entities/property.hpp"

#include <cppbase/types.hpp>

#include <impl/entities/util.hpp>
#include <impl/project.hpp>
#include <impl/util/validation.hpp>

IkarusPropertyCreateV1Result ikarus_property_create_v1_impl(
    Project * project,
    Id template_or_page,
    Id parent_folder,
    size_t position,
    char const * name,
    PropertyType type,
    char const * default_value,
    char const * settings,
    IkarusPropertyCreateV1Flags flags
) {
    IkarusPropertyCreateV1Result ret{.property = ID_NONE, .status_code = StatusCode_Ok};

    Id generated_id = id_generate(project, EntityType_Property);

    Id actual_parent_folder = parent_folder;

    // clang-format off

    VTRYRV(
        ret.property,
        ret,
        project->get_db()->transact([&](sqlitecpp::Database * db) -> cppbase::Result<Id, int> {
            if (id_is_none(actual_parent_folder)) {
                LOG_VERBOSE("parent_folder was passed as none: selecting root folder from DB");

                VTRY(
                    actual_parent_folder,
                    project->get_db()
                        ->get_one<Id>(
                            "SELECT `folder_id` FROM `root_folder_entity_mapping` WHERE `entity_id` = ?",
                            template_or_page
                        )
                        .on_error([&ret] { ret.status_code = StatusCode_InternalError; })
                );
            }

            LOG_VERBOSE("creating property as entity");

            TRY(util::create_entity(project->get_db(), generated_id, actual_parent_folder, position, name, ""));

            LOG_VERBOSE("creating property in db");

            TRY(project->get_db()->exec(
                "INSERT INTO `properties`(`entity_id`, `page_id`, `template_id`, `type`, `default_value`, `settings`) VALUES(?, ?, ?, ?)",
                generated_id,
                id_get_entity_type(template_or_page) == EntityType_Page ? template_or_page : ID_NONE,
                id_get_entity_type(template_or_page) == EntityType_Template ? template_or_page : ID_NONE,
                static_cast<cppbase::u64>(type),
                default_value,
                settings
            ));

            return cppbase::ok(generated_id);
        })
        .on_error([&ret] { ret.status_code = StatusCode_InternalError; })
    );

    // clang-format on

    return ret;
}

cppbase::Result<Id, StatusCode> ikarus_property_create_v1_impl(
    Project * project, Id property, IkarusPropertyDeleteV1Flags flags
) {
    return project->get_db()
        ->exec("DELETE FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<Id, StatusCode> ikarus_property_get_template_v1_impl(
    Project * project, Id property, IkarusPropertyGetTemplateV1Flags flags
) {
    return project->get_db()
        ->get_one<Id>("SELECT `template` FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<Id, StatusCode> ikarus_property_get_page_v1_impl(
    Project * project, Id property, IkarusPropertyGetPageV1Flags flags
) {
    return project->get_db()
        ->get_one<Id>("SELECT `page` FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<PropertyType, StatusCode> ikarus_property_get_type_v1_impl(
    Project * project, Id property, IkarusPropertyGetTypeV1Flags flags
) {
    return project->get_db()
        ->get_one<PropertyType>("SELECT `type` FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<char const *, StatusCode> ikarus_property_get_default_value_v1_impl(
    Project * project, Id property, IkarusPropertyGetDefaultValueV1Flags flags
) {
    return project->get_db()
        ->get_one<char const *>("SELECT `default_value` FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<void, StatusCode> ikarus_property_set_default_value_v1_impl(
    Project * project, Id property, char const * new_default_value, IkarusPropertySetDefaultValueV1Flags flags
) {
    return project->get_db()
        ->exec("UPDATE `properties` SET `default_value` = ? WHERE `id` = ?", new_default_value, property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<char const *, StatusCode> ikarus_property_get_settings_v1_impl(
    Project * project, Id property, IkarusPropertyGetSettingsV1Flags flags
) {
    return project->get_db()
        ->get_one<char const *>("SELECT `settings` FROM `properties` WHERE `id` = ?", property)
        .map_err(StatusCode_InternalError);
}

cppbase::Result<void, StatusCode> ikarus_property_set_settings_v1_impl(
    Project * project, Id property, char const * new_settings, IkarusPropertySetSettingsV1Flags flags
) {
    LOG_VERBOSE("parsing settings to JSON");

    auto settings_json = nlohmann::json::parse(new_settings, nullptr, false);

    // this should never happen
    if (settings_json.is_discarded()) {
        LOG_ERROR("couldn't parse settings as JSON");
        return cppbase::err(StatusCode_InvalidArgument);
    }

    return project->get_db()
        ->transact([project, property, new_settings, flags](sqlitecpp::Database * db) -> cppbase::Result<void, int> {
            VTRY(
                PropertyType type,
                ikarus_property_get_type_v1_wrapper(project, property, IkarusPropertyGetTypeV1Flags_None)
            );
            VTRY(
                char const * default_value,
                ikarus_property_get_default_value_v1_wrapper(
                    project, property, IkarusPropertyGetDefaultValueV1Flags_None
                )
            );

            LOG_VERBOSE("updating settings");

            TRY(db->exec("UPDATE `properties` SET `settings` = ? WHERE `id` = ?", new_settings, property));

            LOG_VERBOSE("verifying default value");

            if (!validate_property_value(type, default_value, new_settings)) {
                LOG_VERBOSE("current default ({}) isn't valid with the new settings. resetting it.", default_value);
                // not using a call here, since the call itself verifies existing values, which we do ourself
                TRY(db->exec(
                    "UPDATE `properties` SET `default_value` = ? WHERE `id` = ?", property_type_get_default_value(type)
                ))
            }

            if ((flags & IkarusPropertySetSettingsV1Flags_ResetInvalidValues) != 0) {
                VTRY(
                    auto values,
                    db->get_many<char const *, Id>(
                        "SELECT `value`, `page_id` FROM `values` WHERE `property_id` = ?", property
                    )
                );

                for (auto const& [value, page] : values) {
                    if (!validate_property_value(type, value, new_settings)) {
                        TRY(db->exec("UPDATE `values` SET `value` = ?"))
                    }
                }
            }
            return cppbase::ok();
        })
        .map_err(StatusCode_InternalError);
}
