#pragma once

#include <sqlite3.h>

#define RETURN_ERROR(ret, err)    \
    do {                          \
        if (err_out != nullptr) { \
            *err_out = err;       \
        }                         \
        return ret;               \
    } while (false)

#define LOG_STD_ERROR(msg)                  LOG_ERROR(msg ". Error({}): {}", ec.value(), ec.message())
#define LOG_SQLITE_ERROR(msg)               LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_RC(msg, rc)        LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_RC_RT(msg, rc)     LOG_ERROR("{}. Error {}({}): {}", msg, rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_DB(msg, db)        LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
#define LOG_SQLITE_ERROR_RC_DB(msg, rc, db) LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
