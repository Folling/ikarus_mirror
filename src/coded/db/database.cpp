#include "database.hpp"

#include "migrations.hpp"
#include "os/fs/temp.hpp"
#include "util.hpp"

namespace db {

Database::Database(sqlite3 * db):
    _db{db} {}

Database::Database(Database&& other) noexcept:
    _db{other._db} {
    other._db = nullptr;
}

Database::~Database() {
    if (_db != nullptr) {
        LOG_FATAL("db handle wasn't nullptr during destruction, forgot to close?");
        // not validating here, if something went wrong this is just used a last resort attempt to close the db properly
        sqlite3_close(_db);
    }
}

Database& Database::operator=(Database&& other) noexcept {
    _db = other._db;
    other._db = nullptr;

    return *this;
}

std::unique_ptr<Database> Database::open(std::filesystem::path const& path, int additional_flags, StatusCode * status_out) {
    LOG_VERBOSE("opening database at {} with the following additional flags {}", path.c_str(), additional_flags);

    if (additional_flags & SQLITE_OPEN_CREATE) {
        if (std::error_code ec{}; std::filesystem::exists(path, ec) && !ec) {
            LOG_WARN("project already exists, aborting creating project");
            RETURN_STATUS_OUT(nullptr, StatusCode_InvalidArgument);
        } else if (ec) {
            LOG_STD_ERROR("unable to check filesystem whether or not the project already exists", ec);
            RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
        }
    }

    sqlite3 * db = nullptr;

    if (auto rc = sqlite3_open_v2(
            path.c_str(),
            &db,
            SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_PRIVATECACHE | additional_flags,
            nullptr
        );
        rc != SQLITE_OK || db == nullptr) {
        LOG_SQLITE_ERROR("unable to initialise sqlite", db, rc);
        RETURN_STATUS_OUT(nullptr, StatusCode_InternalError);
    }

    LOG_VERBOSE("successfully opened database");

    return std::unique_ptr<Database>{new Database{db}};
}

bool Database::migrate(StatusCode * status_out) {
    LOG_VERBOSE("updating database to newest version");

    auto migrations = Migration::get_migrations();

    LOG_INFO("checking current database version");

    VTRYRV(
        bool metadata_table_exists,
        false,
        get_one<int>(status_out, "SELECT EXISTS(SELECT 1 FROM `sqlite_schema` WHERE `tbl_name` = 'metadata')")
    );

    int version;

    if (metadata_table_exists) {
        LOG_VERBOSE("metadata table exists, polling project version");

        VTRYRV(version, false, get_one<int>(status_out, "SELECT `value` FROM `metadata` WHERE `key` = 'VERSION'"));
    } else {
        LOG_VERBOSE("no metadata table exists, assuming project version 0");

        version = 0;
    }

    LOG_VERBOSE("current database version is {}, newest database version is {}", version, migrations.back()->get_version());

    // clang-format off

    if (version > migrations.back()->get_version()) {
        LOG_ERROR("ikarus isn't forward compatible, aborting opening project with future version");
        RETURN_STATUS_OUT(false, StatusCode_InvalidVersion);
        return false;
    }

    TRYRV(false, transact<true>(status_out, std::function{[this, &migrations, version](db::Database& db) -> Result<void, int> {
        if(int rc; this->backup(&rc)) {
            return err(rc);
        }

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
    }}));

    // clang-format on

    return true;
}

bool Database::backup(int * rc_out) {
    LOG_VERBOSE("creating backup database before commencing migrations");
    VTRYRV(auto path, false, os::fs::get_temp_file("ika_migration_backup").inspect_error([&rc_out](int const * err) { *rc_out = *err; }));
    LOG_VERBOSE("storing backup database in {}", path.c_str());

    sqlite3 * backup_handle;
    if (auto rc = sqlite3_open_v2(
            path.c_str(),
            &backup_handle,
            SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_PRIVATECACHE | SQLITE_OPEN_CREATE,
            nullptr
        );
        rc != SQLITE_OK || backup_handle == nullptr) {
        LOG_SQLITE_ERROR("unable to initialise backup db sqlite", backup_handle, rc);
        *rc_out = rc;
        return false;
    }

    sqlite3_backup * bck = sqlite3_backup_init(backup_handle, "MAIN", _db, "MAIN");

    if (bck == nullptr) {
        int rc = sqlite3_errcode(backup_handle);
        LOG_SQLITE_ERROR("unable to initialise sqlite backup handle", backup_handle, rc);
        *rc_out = rc;
        return false;
    }

    if (int rc = sqlite3_backup_step(bck, -1); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to run sqlite backup", backup_handle, rc);
        *rc_out = rc;
        return false;
    }

    if (int rc = sqlite3_backup_finish(bck); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to finalise sqlite backup", backup_handle, rc);
        *rc_out = rc;
        return false;
    }

    return true;
}

bool Database::close(StatusCode * status_out) {
    LOG_VERBOSE("closing database");

    // TODO retry mechanism if busy
    if (auto rc = sqlite3_close_v2(_db); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to close database", _db, rc);
        RETURN_STATUS_OUT(false, StatusCode_InternalError);
    }

    LOG_VERBOSE("database successfully closed, deleting project object");

    _db = nullptr;

    return true;
}

int Database::get_changes() const {
    return sqlite3_changes(_db);
}

}