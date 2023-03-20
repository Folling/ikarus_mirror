#pragma once

#include <sqlite3.h>

#include <filesystem>
#include <mutex>

#include <db/database.hpp>

class DbHandle {
public:
    DbHandle(db::Database * db, std::mutex& mutex):
        _db{db},
        _lock{mutex} {}

public:
    [[nodiscard]] db::Database * get_db() const {
        return _db;
    }

private:
    db::Database * _db;
    std::unique_lock<std::mutex> _lock;
};

class Project {
public:
    Project(std::filesystem::path&& path, std::unique_ptr<db::Database>&& db):
        _path{std::move(path)},
        _db{std::move(db)} {}

public:
    [[nodiscard]] std::filesystem::path const& get_path() const {
        return _path;
    }

    [[nodiscard]] inline DbHandle get_db_handle() const {
        return DbHandle{_db.get(), _db_mutex};
    }

private:
    std::filesystem::path _path;
    std::mutex mutable _db_mutex;
    std::unique_ptr<db::Database> _db;
};
