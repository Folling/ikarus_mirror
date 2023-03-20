#include "statement.hpp"

namespace db {

Statement::Statement(sqlite3_stmt * handle):
    _handle{handle} {}

Statement::Statement(Statement&& other) noexcept:
    _handle{other._handle} {
    other._handle = nullptr;
}

Statement::~Statement() {
    if (int rc = sqlite3_finalize(_handle); rc != SQLITE_OK) {
        LOG_SQLITE_ERROR("unable to finalize statement", nullptr, rc);
    }
}

Statement& Statement::operator=(Statement&& other) noexcept {
    this->_handle = other._handle;
    other._handle = nullptr;

    return *this;
}

}