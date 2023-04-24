#include <iterator>

#include <cppbase/logger.hpp>

#include <sqlitecpp/database.hpp>

#include <ikarus/project.h>
#include <ikarus/status.h>

#include <generated/project.hpp>

#include <impl/db/util.hpp>
#include <impl/project.hpp>

using cppbase::err;
using cppbase::ok;
using cppbase::Result;

Result<Project *, StatusCode> ikarus_project_open_v1_impl(Path path, IkarusProjectOpenV1Flags flags) {
    std::filesystem::path fs_path{path.data};

    auto db = sqlitecpp::Database::open(fs_path, 0);

    if (db == nullptr) {
        return err(StatusCode_InternalError);
    }

    TRY(db->migrate().map_err_with([](auto db_err) {
        switch (db_err) {
        case sqlitecpp::Database::SQLiteCppMigrationError_InvalidVersion: return StatusCode_InvalidVersion;
        case sqlitecpp::Database::SQLiteCppMigrationError_InternalError: return StatusCode_InternalError;
        }
    }));

    cppbase::u32 pid = cppbase::os::ps::get_process_id();

    LOG_VERBOSE("pid (connection-id) is {}", pid);

    // only the first 24 bits may be set, since we're reserving only a total of 24 bits for the "connection-id" in the
    // generation of IDs this should effectively **never** trigger. Windows is the only platform that permits pids > 24
    // bits And there it only happens in extreme circumstances since pids are reused if this does ever become a
    // prevalent problem, consider these alternatives:
    // 1. shared mutex in shared memory used to generate IDs
    // 2. generate ids using an auto-incremented value in DB
    // 3. each Ikarus connection gets allotted 100 million or so IDs as a "space", which is never reused.
    //    This allows for roughly 720,575,940 connections to be made (per project) before running out of space
    if ((pid >> 24) != 0) {
        LOG_ERROR("pid is was greater than 2^24: {}", pid);
        return err(StatusCode_InternalError);
    }

    VTRY(
        auto current_counter,
        // looks more scuffed than it is
        // essentially we're saying "give me the current largest counter for the given connection-id"
        db->get_one<cppbase::u32>(
              "SELECT IFNULL((SELECT MAX((`val` << 32) >> 32) + 1 FROM `foo` WHERE ((`val` << 8) >> 40) == ?), 0)", pid
        )
            .map_err(StatusCode_InternalError)
    );

    LOG_VERBOSE("current counter (id-counter) is {}", current_counter);

    return ok(new Project{std::move(fs_path), std::move(db), pid, current_counter});
}

Result<void, StatusCode> ikarus_project_close_v1_impl(Project * project, IkarusProjectCloseV1Flags flags) {
    if (!project->get_db()->close()) {
        return err(StatusCode_InternalError);
    }

    return ok();
}

Result<void, StatusCode> ikarus_project_delete_v1_impl(Project * project, IkarusProjectDeleteV1Flags flags) {
    // closing project deletes the pointer
    std::filesystem::path path = project->get_path();

    TRY(ikarus_project_close_v1_wrapper(project, IkarusProjectCloseV1Flags_None));

    LOG_VERBOSE("deleting project file");

    if (std::error_code ec{}; std::filesystem::remove(path, ec), ec) {
        LOG_STD_ERROR("unable to remove path from filesystem", ec);
        return err(StatusCode_InternalError);
    }

    return ok();
}

IKA_API Result<void, StatusCode> ikarus_project_get_entities_v1_impl(
    Project const * project,
    Id * entities_out,
    size_t entities_out_size,
    EntityTypes entity_types,
    IkarusProjectGetEntitiesV1Flags flags
) {
    TRY(project->get_db()
            ->get_many_buffered<Id>(
                "SELECT `id` FROM `entities` WHERE (((`id` >> 56) & ?) != 0) LIMIT ?",
                entities_out,
                entities_out_size,
                static_cast<int>(entity_types),
                entities_out_size
            )
            .map_err(StatusCode_InternalError));

    return ok();
}

IKA_API Result<std::size_t, StatusCode> ikarus_project_get_entities_count_v1_impl(
    Project const * project, EntityTypes entity_types, IkarusProjectGetEntitiesCountV1Flags flags
) {
    return project->get_db()
        ->get_one<size_t>(
            "SELECT COUNT(*) FROM `entities` WHERE (((`id` >> 56) & ?) != 0)", static_cast<int>(entity_types)
        )
        .map_err(StatusCode_InternalError);
}
