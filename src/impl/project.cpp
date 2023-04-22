#include <iterator>

#include <cppbase/logger.hpp>

#include <sqlitecpp/database.hpp>

#include <ikarus/project.h>
#include <ikarus/status.h>

#include <generated/project.hpp>

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

    ret.project = new Project{std::move(fs_path), std::move(db)};

    return ret;
}

IkarusProjectCloseV1Result ikarus_project_close_v1_impl(Project * project, IkarusProjectCloseV1Flags flags) {
    IkarusProjectCloseV1Result ret{.status_code = StatusCode_Ok};

    if (!project->get_db().close()) {
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
            .get_many_buffered<Id>(
                "SELECT `id` FROM `entities` WHERE (((`id` >> 56) & ?) != 0) LIMIT ?",
                entities_out,
                entities_out_size,
                static_cast<int>(entity_types),
                entities_out_size
            )
            .inspect_error([&ret](auto const& _) { ret.status_code = StatusCode_InternalError; })
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
            .get_one<size_t>("SELECT COUNT(*) FROM `entities` WHERE (((`id` >> 56) & ?) != 0)", static_cast<int>(entity_types))
            .inspect_error([&ret](auto const& _) { ret.status_code = StatusCode_InternalError; })
    );

    return ret;
}
