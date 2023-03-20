#include "blueprint.hpp"

#include "db/db.hpp"

#include <error.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <entities/folder.hpp>
#include <error.hpp>
#include <project.hpp>

Id ikarus_blueprint_create_v1(
    Project * project, Id parent_folder, char const * name, IkarusBlueprintCreateV1Flags flags, ErrorCode * err_out
) {
    if (project == nullptr) {
        RETURN_ERROR(id_null(), ErrorCode_NullSubject);
    }

    LOG_INFO("creating blueprint in parent folder {} with name {} and flags {}", parent_folder, name, flags);

    Id id = id_generate(EntityType_Blueprint);

    TRYRVE(
        id_null(),
        ErrorCode_DatabaseError,
        db::exec_log(
            project->db,
            "unable to insert blueprint into entities table",
            "INSERT INTO `entities`(`id`, `name`, `information`) VALUES(?, ?, ?)",
            id,
            name,
            ""
        )
    );

    return id;
}
