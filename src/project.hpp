#pragma once

#include <sqlite3.h>

#include <filesystem>
#include <mutex>

class DbHandle {
public:
    DbHandle(sqlite3 * db, std::mutex& mutex):
        _db{db},
        _lock{mutex} {}

public:
    [[nodiscard]] sqlite3 * get_db() const {
        return _db;
    }

private:
    sqlite3 * _db;
    std::unique_lock<std::mutex> _lock;
};

class Project {
public:
    Project(std::filesystem::path&& path, sqlite3 * db):
        _path{std::move(path)},
        _db{db} {}

public:
    [[nodiscard]] std::filesystem::path const& get_path() const {
        return _path;
    }

    [[nodiscard]] inline DbHandle get_db_handle() const {
        return DbHandle{_db, _db_mutex};
    }

private:
    std::filesystem::path _path;
    std::mutex mutable _db_mutex;
    sqlite3 * _db;
};
