#pragma once

#include <sqlite3.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include <status.hpp>
#include <util/structs/result.hpp>

namespace db {

class Database {
public:
    Database(Database const& other) = delete;
    Database(Database&& other) noexcept;
    ~Database();

    Database& operator=(Database const& other) = delete;
    Database& operator=(Database&& other) noexcept;

    [[nodiscard]] static std::unique_ptr<Database> open(std::filesystem::path const& path, int additional_flags, StatusCode * status_out);
    [[nodiscard]] bool close(StatusCode * status_out);

private:
    explicit Database(sqlite3 * db);

public:
    [[nodiscard]] bool migrate(StatusCode * status_out);

public:
    template<typename... Ret, typename... Args>
    [[nodiscard]] Result<tmpl::maybe_tuple<tmpl::remove_all<Ret>...>, int> get_one(
        StatusCode * status_out, std::string_view view, Args&&... args
    ) const;

    template<typename T, typename... Args>
    [[nodiscard]] Result<size_t, int> get_many_buffered(
        StatusCode * status_out, std::string_view view, T * out_arr, size_t out_arr_size, Args&&... args
    ) const;

    template<typename... Ret, typename... Args>
    [[nodiscard]] Result<std::vector<tmpl::maybe_tuple<tmpl::remove_all<Ret>...>>, int> get_many(
        StatusCode * status_out, std::string_view view, Args&&... args
    ) const;

    template<typename... Args>
    [[nodiscard]] Result<void, int> exec(StatusCode * status_out, std::string_view view, Args&&... args);

    template<bool exclusive, typename F>
    [[nodiscard]] Result<void, int> transact(StatusCode * status_out, F func);

public:
    [[nodiscard]] int get_changes() const;

private:
    template<typename... Ret, typename FCond, typename FIns, typename... Args>
    [[nodiscard]] Result<void, int> _get_many_impl(StatusCode * status_out, std::string_view view, FCond cond, FIns ins, Args&&... args)
        const;

private:
    sqlite3 * _db;
};

}
