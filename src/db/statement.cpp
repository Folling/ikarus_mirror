#include "statement.ipp"

namespace db {

Statement::Statement():
    _handle{nullptr} {}

Statement::Statement(Statement&& other) noexcept:
    _handle{other._handle} {
    other._handle = nullptr;
}

Statement::~Statement() {
    // save no-op for nulls
    sqlite3_finalize(_handle);
}

Statement& Statement::operator=(Statement&& other) noexcept {
    _handle = other._handle;
    other._handle = nullptr;

    return *this;
}

}