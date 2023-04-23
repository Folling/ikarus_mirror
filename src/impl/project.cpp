#include <iterator>

#include <cppbase/logger.hpp>

#include <sqlitecpp/database.hpp>

#include <ikarus/project.h>
#include <ikarus/status.h>

#include <generated/project.hpp>

#include <impl/db/util.hpp>
#include <impl/project.hpp>
#include <impl/status.hpp>

IKA_API IkarusProjectOpenV1Result ikarus_project_open_v1_impl(Path path, IkarusProjectOpenV1Flags flags) {
    IkarusProjectOpenV1Result ret{.project = nullptr, .status_code = StatusCode_Ok};

    std::filesystem::path fs_path{path.data};

    auto db = sqlitecpp::Database::open(fs_path, 0);

    if (db == nullptr) {
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    if (!db->migrate()) {
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    cppbase::u32 pid = cppbase::os::ps::get_process_id();

    LOG_VERBOSE("pid (connection-id) is {}", pid);

    // only the first 24 bits may be set, since we're reserving only a total of 24 bits for the "connection-id" in the generation of IDs
    // this should effectively **never** trigger. Windows is the only platform that permits pids > 24 bits
    // And there it only happens in extreme circumstances since pids are reused
    // if this does ever become a prevalent problem, consider these alternatives:
    // 1. shared mutex in shared memory used to generate IDs
    // 2. generate ids using an auto-incremented value in DB
    // 3. each Ikarus connection gets allotted 100 million or so IDs as a "space", which is never reused.
    //    This allows for roughly 720,575,940 connections to be made (per project) before running out of space
    if ((pid >> 24) != 0) {
        LOG_ERROR("pid is was greater than 2^24: {}", pid);
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    VTRYRV(
        auto current_counter,
        ret,
        // looks more scuffed than it is
        // essentially we're saying "give me the maximum ID for the given connection-id"
        db->get_one<cppbase::u32>("SELECT IFNULL((SELECT MAX((`val` << 32) >> 32) + 1 FROM `foo` WHERE ((`val` << 8) >> 40) == ?), 0)", pid)
            .on_error([&ret] { ret.status_code = StatusCode_InternalError; })
    );

    LOG_VERBOSE("current counter (id-counter) is {}", current_counter);

    ret.project = new Project{std::move(fs_path), std::move(db), pid, current_counter};

    return ret;
}

IkarusProjectCloseV1Result ikarus_project_close_v1_impl(Project * project, IkarusProjectCloseV1Flags flags) {
    IkarusProjectCloseV1Result ret{.status_code = StatusCode_Ok};

    if (!project->get_db()->close()) {
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    return ret;
}

IkarusProjectDeleteV1Result ikarus_project_delete_v1_impl(Project * project, IkarusProjectDeleteV1Flags flags) {
    IkarusProjectDeleteV1Result ret{.status_code = StatusCode_Ok};

    // closing project deletes the pointer
    std::filesystem::path path = project->get_path();

    if (IkarusProjectCloseV1Result close_ret = ikarus_project_close_v1(project, IkarusProjectCloseV1Flags_None);
        close_ret.status_code != StatusCode_Ok) {
        RETURN_STATUS(ret, close_ret.status_code);
    }

    LOG_VERBOSE("deleting project file");

    if (std::error_code ec{}; std::filesystem::remove(path, ec), ec) {
        LOG_STD_ERROR("unable to remove path from filesystem", ec);
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    return ret;
}

IKA_API IkarusProjectGetEntitiesV1Result ikarus_project_get_entities_v1(
    Project const * project, Id * entities_out, size_t entities_out_size, EntityTypes entity_types, IkarusProjectGetEntitiesV1Flags flags
) {
    IkarusProjectGetEntitiesV1Result ret{.status_code = StatusCode_Ok};

    TRYRV(
        ret,
        project->get_db()
            ->get_many_buffered<Id>(
                "SELECT `id` FROM `entities` WHERE (((`id` >> 56) & ?) != 0) LIMIT ?",
                entities_out,
                entities_out_size,
                static_cast<int>(entity_types),
                entities_out_size
            )
            .on_error([&ret] { ret.status_code = StatusCode_InternalError; })
    );

    return ret;
}

IKA_API IkarusProjectGetEntitiesCountV1Result
ikarus_project_get_entities_count_v1(Project const * project, EntityTypes entity_types, IkarusProjectGetEntitiesCountV1Flags flags) {
    IkarusProjectGetEntitiesCountV1Result ret{.count = 0, .status_code = StatusCode_Ok};

    VTRYRV(
        ret.count,
        ret,
        project->get_db()
            ->get_one<size_t>("SELECT COUNT(*) FROM `entities` WHERE (((`id` >> 56) & ?) != 0)", static_cast<int>(entity_types))
            .on_error([&ret] { ret.status_code = StatusCode_InternalError; })
    );

    return ret;
}
