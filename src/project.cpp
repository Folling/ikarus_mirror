#include "project.hpp"

#include <iterator>

#include <db/db.hpp>
#include <db/migrations.hpp>
#include <util/logger.hpp>

#define CHECK_AND_LOG_SUBJECT() CHECK_AND_LOG_SUBJECT_IMPL(project, project->path.c_str())

Project * project_open_impl(char const * path, int additional_flags, ErrorCode * err_out) {
    if (path == nullptr) {
        LOG_WARN("passed path was null");
        RETURN_ERROR(nullptr, ErrorCode_NullArgument);
    }

    LOG_TRACE("commencing opening project at \"{}\"", path);

    std::filesystem::path fs_path = path;

    if (additional_flags & SQLITE_OPEN_CREATE) {
        if (std::error_code ec{}; std::filesystem::exists(fs_path, ec) && !ec) {
            LOG_WARN("project already exists, aborting creating project");
            RETURN_ERROR(nullptr, ErrorCode_AlreadyExists);
        } else if (ec) {
            LOG_STD_ERROR("unable to check filesystem whether or not the project already exists");
            RETURN_ERROR(nullptr, ErrorCode_FilesystemError);
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
        RETURN_ERROR(nullptr, ErrorCode_DatabaseError);
    }

    LOG_INFO("successfully opened project");

    auto migrations = Migration::get_migrations();

    int version;

    if (additional_flags & SQLITE_OPEN_CREATE) {
        LOG_INFO("no metadata table exists, which is fine for an empty project, commencing genesis migration");
        version = 0;
    } else {
        LOG_INFO("checking current database version");

        EVTRYRVE(
            version,
            nullptr,
            ErrorCode_DatabaseError,
            db::get_one<int>(
                db,
                "SELECT IF("
                "EXISTS(SELECT 1 FROM `sqlite_schema` WHERE `tbl_name` = 'metadata'), "
                "(SELECT `value` FROM `metadata` WHERE `key` = 'VERSION')), "
                "0"
                ") "
            )
        );

        if (version == 0) {
            LOG_ERROR("no metadata table exists for an existing project");
            RETURN_ERROR(nullptr, ErrorCode_InvalidDatabaseVersion);
        }
    }

    LOG_VERBOSE("current database version is {}, newest database version is {}", version, migrations.back()->get_version());

    if (auto res = db::transact<true>(
            db,
            "unable to run migrations",
            [&migrations, version](sqlite3 * db) -> Result<void, int> {
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
            }
        );
        res.is_err()) {
        RETURN_ERROR(nullptr, ErrorCode_MigrationFailed);
    }

    return new Project{.path = std::move(fs_path), .db = db};
}

Project * ikarus_project_create_v1(char const * path, IkarusProjectCreateV1Flags flags, ErrorCode * err_out) {
    LOG_FUNCTION("creating project");

    return project_open_impl(path, SQLITE_OPEN_CREATE, err_out);
}

Project * ikarus_project_open_v1(char const * path, IkarusProjectOpenV1Flags flags, ErrorCode * err_out) {
    LOG_FUNCTION("opening project");

    return project_open_impl(path, 0, err_out);
}

bool ikarus_project_close_v1(Project * project, IkarusProjectCloseV1Flags flags, ErrorCode * err_out) {
    LOG_FUNCTION_CHECK_SUBJECT("closing project");

    LOG_TRACE("closing database");

    if (auto rc = sqlite3_close_v2(project->db); rc == SQLITE_OK) {
        LOG_VERBOSE("database successfully closed, deleting project object");

        delete project;
    } else {
        LOG_SQLITE_ERROR_DB("unable to close database", project->db);
        RETURN_ERROR(false, ErrorCode_DatabaseError);
    }

    return true;
}

bool ikarus_project_delete_v1(Project * project, IkarusProjectDeleteV1Flags flags, ErrorCode * err_out) {
    LOG_FUNCTION_CHECK_SUBJECT("deleting project");

    // closing project deletes the pointer
    std::filesystem::path path = project->path;

    if (!ikarus_project_close_v1(project, static_cast<IkarusProjectCloseV1Flags>(0), err_out)) {
        return false;
    }

    LOG_VERBOSE("deleting project file");

    if (std::error_code ec{}; std::filesystem::remove(path, ec), ec) {
        LOG_STD_ERROR("unable to remove path from filesystem");
        RETURN_ERROR(false, ErrorCode_FilesystemError);
    }

    return true;
}

bool ikarus_project_get_blueprints_v1(
    Project const * project, Id * blueprints_out, size_t blueprints_out_size, IkarusProjectGetBlueprintsV1Flags flags, ErrorCode * err_out
) {
    LOG_FUNCTION_CHECK_SUBJECT("getting project blueprints");

    TRYRVE(
        false,
        ErrorCode_DatabaseError,
        db::get_many_buffered_log<Id>(
            project->db, "unable to fetch all blueprint ids", "SELECT `id` FROM `blueprints`", blueprints_out, blueprints_out_size
        )
    );

    return true;
}

size_t ikarus_project_get_blueprints_count_v1(Project const * project, IkarusProjectGetBlueprintsCountV1Flags flags, ErrorCode * err_out) {
    LOG_FUNCTION_CHECK_SUBJECT("getting project blueprints count");

    VTRYRVE(
        ret,
        0,
        ErrorCode_DatabaseError,
        db::get_one_log<size_t>(project->db, "unable to fetch blueprints count", "SELECT COUNT(*) FROM `blueprints`")
    );

    return ret;
}
