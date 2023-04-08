#pragma once

#include "statement.hpp"
#include "status.hpp"
#include "util/structs/result.hpp"

#include <sqlite3.h>

#include <filesystem>
#include <string_view>
#include <vector>

#include <fmt/format.h>

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
    [[nodiscard]] bool backup(int * rc_out);

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

template<typename... Ret, typename... Args>
Result<tmpl::maybe_tuple<tmpl::remove_all<Ret>...>, int> Database::get_one(StatusCode * status_out, std::string_view view, Args&&... args)
    const {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(Statement stmt, Statement::prepare(_db, status_out, view, std::forward<Args>(args)...));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt._handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_DONE: {
        LOG_SQLITE_ERROR("statement step finished prematurely", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    }

    tmpl::maybe_tuple<tmpl::remove_all<Ret>...> ret = detail::convert<Ret...>(stmt._handle);

    LOG_TRACE("successfully ran statement");

    return ok(std::move(ret));
}

template<typename... Ret, typename FCond, typename FIns, typename... Args>
Result<void, int> Database::_get_many_impl(StatusCode * status_out, std::string_view view, FCond cond, FIns ins, Args&&... args) const {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(Statement stmt, Statement::prepare(_db, status_out, view, std::forward<Args>(args)...));

    size_t i = 0;

    while (cond(i)) {
        int rc = SQLITE_OK;
        switch (rc = sqlite3_step(stmt._handle)) {
        case SQLITE_DONE: {
            goto done;
        }
        case SQLITE_ROW: {
            ins(i++, std::move(detail::convert<Ret...>(stmt._handle)));
            break;
        }
        default: {
            LOG_SQLITE_ERROR("unknown step error", _db, rc);
            RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
        }
        }
    }

done:
    LOG_TRACE("successfully ran statement");

    return ok();
}

template<typename T, typename... Args>
Result<size_t, int> Database::get_many_buffered(
    StatusCode * status_out, std::string_view view, T * out_arr, size_t out_arr_size, Args&&... args
) const {
    return _get_many_impl<tmpl::remove_all<T>>(
        status_out,
        view,
        [out_arr_size](int i) { return i < out_arr_size; },
        [out_arr](size_t i, tmpl::maybe_tuple<tmpl::remove_all<T>>&& val) { out_arr[i] = val; },
        std::forward<Args>(args)...
    );
}

template<typename... Ret, typename... Args>
Result<std::vector<tmpl::maybe_tuple<tmpl::remove_all<Ret>...>>, int> Database::get_many(
    StatusCode * status_out, std::string_view view, Args&&... args
) const {
    std::vector<tmpl::maybe_tuple<tmpl::remove_all<Ret>...>> ret{};

    TRY(_get_many_impl<tmpl::remove_all<Ret>...>(
        status_out,
        view,
        [](int i) { return true; },
        [&ret](size_t i, tmpl::maybe_tuple<tmpl::remove_all<Ret>...>&& val) { ret.emplace_back(std::move(val)); },
        std::forward<Args>(args)...
    ));

    return ok(ret);
}

template<typename... Args>
Result<void, int> Database::exec(StatusCode * status_out, std::string_view view, Args&&... args) {
    LOG_TRACE("executing statement:\n{}", view);

    if constexpr (sizeof...(Args) != 0) {
        LOG_TRACE("args are: {}", fmt::format(fmt::runtime(format::format_str<Args...>()), std::forward<Args>(args)...));
    }

    VTRY(Statement stmt, Statement::prepare(_db, status_out, view, std::forward<Args>(args)...));

    LOG_TRACE("expanded sqlite statement to:\n{}", sqlite3_expanded_sql(stmt._handle));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt._handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_DONE: [[fallthrough]];
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step", _db, rc);
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    }

    LOG_TRACE("successfully executed statement");

    return ok();
}

template<bool exclusive, typename F>
Result<void, int> Database::transact(StatusCode * status_out, F func) {
    if constexpr (exclusive) {
        TRY(exec(status_out, "BEGIN EXCLUSIVE TRANSACTION;"));
    } else {
        TRY(exec(status_out, "BEGIN TRANSACTION;"));
    }

    if (auto res = func(*this); res.is_err()) {
        LOG_SQLITE_ERROR("unable to run transaction", _db, res.unwrap_error());
        auto _ = exec(status_out, "ROLLBACK TRANSACTION;");
        return res;
    }

    TRY(exec(status_out, "COMMIT TRANSACTION;"));

    return ok();
}

}
