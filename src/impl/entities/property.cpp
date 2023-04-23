#include "generated/entities/property.hpp"

#include <cppbase/types.hpp>

#include <impl/entities/util.hpp>
#include <impl/project.hpp>

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

IkarusPropertyDeleteV1Result ikarus_property_delete_v1_impl(
    Project * project, Id property, IkarusPropertyDeleteV1Flags flags
);

IkarusPropertyGetTemplateV1Result ikarus_property_get_template_v1_impl(
    Project * project, Id property, IkarusPropertyGetTemplateV1Flags flags
);

IkarusPropertyGetPageV1Result ikarus_property_get_page_v1_impl(
    Project * project, Id property, IkarusPropertyGetPageV1Flags flags
);

IkarusPropertyGetTypeV1Result ikarus_property_get_type_v1_impl(
    Project * project, Id property, IkarusPropertyGetTypeV1Flags flags
);

IkarusPropertyGetDefaultValueV1Result ikarus_property_get_default_value_v1_impl(
    Project * project, Id property, IkarusPropertyGetDefaultValueV1Flags flags
);

IkarusPropertySetDefaultValueV1Result ikarus_property_set_default_value_v1_impl(
    Project * project, Id property, char const * new_default_value, IkarusPropertySetDefaultValueV1Flags flags
);
