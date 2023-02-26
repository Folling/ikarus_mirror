#include "ikarus/project.h"

#include "project.hpp"

#include <cstdio>
#include <iterator>

#include <db/db.hpp>
#include <db/migrations.hpp>
#include <ikarus/status.h>
#include <util/logger.hpp>
#include <validation/arg.hpp>

Project * project_open_impl(char const * path, int additional_flags, StatusCode * status_out) {
    LOG_TRACE("commencing opening project at \"{}\"", path);

    std::filesystem::path fs_path = path;

    if (additional_flags & SQLITE_OPEN_CREATE) {
        if (std::error_code ec{}; std::filesystem::exists(fs_path, ec) && !ec) {
            LOG_WARN("project already exists, aborting creating project");
            RETURN_STATUS_OUT(nullptr, StatusCode_Duplicate);
        } else if (ec) {
            LOG_STD_ERROR("unable to check filesystem whether or not the project already exists");
            RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
        }
    }

    LOG_VERBOSE("opening database with the following additional flags: {}", additional_flags);

    sqlite3 * db = nullptr;

    if (auto rc = sqlite3_open_v2(
            fs_path.c_str(),
            &db,
            SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_PRIVATECACHE | additional_flags,
            nullptr
        );
        rc != SQLITE_OK || db == nullptr) {
        LOG_SQLITE_ERROR("unable to initialise sqlite");
        RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
    }

    LOG_INFO("successfully opened project");

    auto migrations = Migration::get_migrations();

    int version;

    if (additional_flags & SQLITE_OPEN_CREATE) {
        LOG_INFO("no metadata table exists, which is fine for an empty project, commencing genesis migration");
        version = 0;
    } else {
        LOG_INFO("checking current database version");

        VTRYRV(
            version,
            nullptr,
            db::get_one<int>(
                db,
                status_out,
                "SELECT IF("
                "EXISTS(SELECT 1 FROM `sqlite_schema` WHERE `tbl_name` = 'metadata'), "
                "(SELECT `value` FROM `metadata` WHERE `key` = 'VERSION')), "
                "0"
                ") "
            )
        );

        if (version == 0) {
            LOG_ERROR("no metadata table exists for an existing project");
            RETURN_STATUS_OUT(nullptr, StatusCode_Corrupted);
        }
    }

    LOG_VERBOSE("current database version is {}, newest database version is {}", version, migrations.back()->get_version());

    TRYRV(nullptr, db::transact<true>(db, status_out, [&migrations, version](sqlite3 * db) -> Result<void, int> {
              for (auto const& migration : migrations) {
                  if (auto migration_version = migration->get_version(); migration_version > version) {
                      LOG_INFO("running migration {}", migration_version);
                      TRY(migration->up(db));
                      LOG_VERBOSE("successfully ran migration");
                  } else {
                      break;
                  }
              }

              LOG_INFO("successfully ran all migrations");
              return ok();
          }));

    LOG_INFO("project is now up to date");

    return (new Project{.path = std::move(fs_path), .db = db});
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

    LOG_VERBOSE("closing database");

    if (auto rc = sqlite3_close_v2(project->db); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR_DB("unable to close database", project->db);
        RETURN_STATUS(ret, StatusCode_InternalError);
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
    std::filesystem::path path = project->path;

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

    VTRYRV(
        ret.count,
        ret,
        db::get_many_buffered<Id>(project->db, &ret.status_code, "SELECT `id` FROM `blueprints`", blueprints_out, blueprints_out_size)
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

    VTRYRV(ret.count, ret, db::get_one<size_t>(project->db, &ret.status_code, "SELECT COUNT(*) FROM `blueprints`"));

    LOG_FUNCTION_SUCCESS("successfully fetched blueprints count");

    return ret;
}
