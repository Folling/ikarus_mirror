#pragma once

#include <sqlite3.h>

#include <atomic>
#include <filesystem>
#include <mutex>
#include <shared_mutex>

#include <impl/db/database.hpp>
#include <impl/util/types.hpp>

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
        _connection_id{},
        _id_counter{},
        _path{std::move(path)},
        _db{std::move(db)} {}

public:
    [[nodiscard]] std::filesystem::path const& get_path() const {
        return _path;
    }

    [[nodiscard]] u16 get_connection_id() const {
        return _connection_id;
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

    std::shared_mutex _db_mutex;
    std::shared_lock _read_lock;
    std::unique_ptr<db::Database> _db;
};
