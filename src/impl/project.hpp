#pragma once

#include "db/database.hpp"
#include "util/types.hpp"

#include <sqlite3.h>

#include <atomic>
#include <filesystem>
#include <mutex>

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

    [[nodiscard]] std::atomic<u64>& get_connection_id() {
        return _id_counter;
    }

    [[nodiscard]] std::atomic<u64>& get_id_counter() {
        return _id_counter;
    }

    [[nodiscard]] inline DbHandle get_db_handle() const {
        return DbHandle{_db.get(), _db_mutex};
    }

private:
    std::filesystem::path _path;

    u16 _connection_id;
    std::atomic<u64> _id_counter;

    std::mutex mutable _db_mutex;
    std::unique_ptr<db::Database> _db;
};
