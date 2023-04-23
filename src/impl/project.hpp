#pragma once

#include <sqlite3.h>

#include <atomic>
#include <filesystem>
#include <mutex>
#include <shared_mutex>

#include <cppbase/os/ps.hpp>
#include <cppbase/types.hpp>

#include <sqlitecpp/database.hpp>

class Project {
public:
    Project(std::filesystem::path&& path, std::unique_ptr<sqlitecpp::Database>&& db, cppbase::u32 connection_id, cppbase::u32 id_counter):
        _path{std::move(path)},
        _db{std::move(db)},
        _connection_id{connection_id},
        _id_counter{} {}

public:
    [[nodiscard]] std::filesystem::path const& get_path() const {
        return _path;
    }

    [[nodiscard]] cppbase::u16 get_connection_id() const {
        return _connection_id;
    }

    [[nodiscard]] std::atomic<cppbase::u64>& get_id_counter() {
        return _id_counter;
    }

    [[nodiscard]] inline sqlitecpp::Database * get_db() {
        return _db.get();
    }

    [[nodiscard]] inline sqlitecpp::Database const * get_db() const {
        return _db.get();
    }

    [[nodiscard]] inline std::shared_mutex& get_db_mutex() const {
        return _db_mutex;
    }

private:
    std::filesystem::path _path;
    std::unique_ptr<sqlitecpp::Database> _db;

    // mutable required for read locks with a const project pointer
    std::shared_mutex mutable _db_mutex;

    cppbase::u32 _connection_id;
    std::atomic<cppbase::u64> _id_counter;
};
