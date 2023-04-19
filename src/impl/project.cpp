#include <iterator>

#include <ikarus/status.h>
#include <impl/db/database.hpp>
#include <impl/ikarus/project.h>
#include <impl/project.hpp>
#include <impl/util/logger.hpp>
#include <impl/validation/arg.hpp>

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

IkarusProjectCreateV1Result ikarus_project_create_v1(char const * path, IkarusProjectCreateV1Flags flags) {
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
        LOG_STD_ERROR("unable to remove path from filesystem", ec);
        RETURN_STATUS(ret, StatusCode_InternalError);
    }

    LOG_FUNCTION_SUCCESS("successfully deleted project");

    return ret;
}

IkarusProjectGetTemplatesResult ikarus_project_get_templates_v1(
    Project const * project, Id * templates_out, size_t templates_out_size, IkarusProjectGetTemplatesV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching project templates");

    IkarusProjectGetTemplatesResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    auto db_handle = project->get_db_handle();

    VTRYRV(
        ret.count,
        ret,
        db_handle.get_db()->get_many_buffered<Id>(&ret.status_code, "SELECT `id` FROM `templates`", templates_out, templates_out_size)
    );

    LOG_FUNCTION_SUCCESS("successfully fetched templates");

    return ret;
}

IkarusProjectGetTemplatesCountResult ikarus_project_get_templates_count_v1(
    Project const * project, IkarusProjectGetTemplatesCountV1Flags flags
) {
    LOG_FUNCTION_VERBOSE("fetching project templates count");

    IkarusProjectGetTemplatesCountResult ret{.count = 0, .status_code = StatusCode_Ok};

    CHECK(ret, validation::validate_project<validation::NotNull | validation::Exists>(project, "project", &ret.status_code));

    auto db_handle = project->get_db_handle();

    VTRYRV(ret.count, ret, db_handle.get_db()->get_one<size_t>(&ret.status_code, "SELECT COUNT(*) FROM `templates`"));

    LOG_FUNCTION_SUCCESS("successfully fetched templates count");

    return ret;
}
