#include "ikarus/project.h"

#include "project.hpp"

#include <iterator>

#include <db/database.ipp>
#include <ikarus/status.h>
#include <util/logger.hpp>
#include <validation/arg.hpp>

Project * project_open_impl(char const * path, int additional_flags, StatusCode * status_out) {
    std::filesystem::path fs_path{path};

    auto db = db::Database::open(fs_path, additional_flags, status_out);

    if (db == nullptr) {
        return nullptr;
    }

    if (!db->migrate(status_out)) {
        return nullptr;
    }

    return (new Project{std::move(fs_path), std::move(db)});
}

IkarusProjectCreateResult ikarus_project_create_v1(char const * path, IkarusProjectCreateV1Flags flags) {
    LOG_FUNCTION_INFO("creating project");
    IkarusProjectCreateResult ret{.project = nullptr, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_path<validation::NotNull | validation::DoesntExist>(path, "project path", &ret.status_code));

    ret.project = project_open_impl(path, SQLITE_OPEN_CREATE, &ret.status_code);

    LOG_FUNCTION_SUCCESS("successfully created project");

    return ret;
}

IkarusProjectOpenResult ikarus_project_open_v1(char const * path, IkarusProjectOpenV1Flags flags) {
    LOG_FUNCTION_INFO("opening project");
    IkarusProjectOpenResult ret{.project = nullptr, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_path<validation::NotNull | validation::Exists>(path, "project path", &ret.status_code));

    ret.project = project_open_impl(path, 0, &ret.status_code);

    LOG_FUNCTION_SUCCESS("successfully opened project");

    return ret;
}

IkarusProjectCloseResult ikarus_project_close_v1(Project * project, IkarusProjectCloseV1Flags flags) {
    LOG_FUNCTION_VERBOSE("closing project");
    IkarusProjectCloseResult ret{.status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    auto db_handle = project->get_db_handle();

    if (!db_handle.get_db()->close(&ret.status_code)) {
        return ret;
    }

    LOG_VERBOSE("database successfully closed, deleting project object");

    delete project;

    LOG_FUNCTION_SUCCESS("successfully closed project");

    return ret;
}

IkarusProjectDeleteResult ikarus_project_delete_v1(Project * project, IkarusProjectDeleteV1Flags flags) {
    LOG_FUNCTION_INFO("deleting project");
    IkarusProjectDeleteResult ret{.status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    // closing project deletes the pointer
    std::filesystem::path path = project->get_path();

    if (IkarusProjectCloseResult close_ret = ikarus_project_close_v1(project, static_cast<IkarusProjectCloseV1Flags>(0));
        close_ret.status_code != StatusCode_Ok) {
        RETURN_STATUS(ret, close_ret.status_code);
    }

    LOG_VERBOSE("deleting project file");

    if (std::error_code ec{}; std::filesystem::remove(path, ec), ec) {
        LOG_STD_ERROR("unable to remove path from filesystem");
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    LOG_FUNCTION_SUCCESS("successfully deleted project");

    return ret;
}

IkarusProjectGetBlueprintsResult ikarus_project_get_blueprints_v1(
    Project const * project, Id * blueprints_out, size_t blueprints_out_size, IkarusProjectGetBlueprintsV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching project blueprints");

    IkarusProjectGetBlueprintsResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    auto db_handle = project->get_db_handle();

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_many_buffered<Id>(&ret.status_code, "SELECT `id` FROM `blueprints`", blueprints_out, blueprints_out_size)
    );

    LOG_FUNCTION_SUCCESS("successfully fetched blueprints");

    return ret;
}

IkarusProjectGetBlueprintsCountResult ikarus_project_get_blueprints_count_v1(
    Project const * project, IkarusProjectGetBlueprintsCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching project blueprints count");

    IkarusProjectGetBlueprintsCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    auto db_handle = project->get_db_handle();

    VTRYRV(ret.count, ret, db_handle.get_db()->get_one<size_t>(&ret.status_code, "SELECT COUNT(*) FROM `blueprints`"));

    LOG_FUNCTION_SUCCESS("successfully fetched blueprints count");

    return ret;
}
