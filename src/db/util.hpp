#pragma once

#include <util/logger.hpp>

#define LOG_SQLITE_ERROR(msg, db, rc) LOG_ERROR(msg ". Error {}({}): {}", rc, sqlite3_errstr(rc), sqlite3_errmsg(db))
