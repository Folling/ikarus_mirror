#pragma once

#include <sqlite3.h>

#include <ikarus/status.h>

inline bool is_status(StatusCode const * status, StatusCode sc) {
    return status != nullptr && *status == sc;
}

inline void set_status(StatusCode * status_out, StatusCode sc) {
    if (status_out != nullptr) {
        *status_out = sc;
    }
}

#define RETURN_STATUS_OUT(ret, sc) \
    set_status(status_out, sc);    \
    return ret

#define RETURN_STATUS(ret, sc) \
    (ret).status_code = sc;    \
    return ret

#define LOG_STD_ERROR(msg)                  LOG_ERROR(msg ". Error({}): {}", ec.value(), ec.message())
#define LOG_STD_ERROR_F(msg, ...)           LOG_ERROR(msg ". Error({}): {}", __VA_ARGS__, ec.value(), ec.message())
#define LOG_SQLITE_ERROR(msg)               LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_RC(msg, rc)        LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_DB(msg, db)        LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_RC_DB(msg, rc, db) LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
