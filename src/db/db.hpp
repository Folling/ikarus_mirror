#pragma once

#include "util/status.hpp"

#include <sqlite3.h>

#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <db/sqlitehelper.hpp>
#include <util/format.hpp>
#include <util/logger.hpp>
#include <util/structs/result.hpp>
#include <util/templates.hpp>

namespace db {

template<typename... Args>
Result<void, int> exec(sqlite3 * db, StatusCode * status_out, std::string_view view, Args&&... args) {
    LOG_TRACE("executing statement:\n{}", view);

    if constexpr (sizeof...(Args) != 0) {
        LOG_TRACE("args are: {}", fmt::format(fmt::runtime(format_str<Args...>()), std::forward<Args>(args)...));
    }

    VTRY(auto stmt, detail::prepare(db, status_out, view, std::forward<Args>(args)...));

    LOG_TRACE("expanded sqlite statement to:\n{}", sqlite3_expanded_sql(stmt.handle));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt.handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_DONE: [[fallthrough]];
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    }

    LOG_TRACE("successfully executed statement");

    return ok();
}

template<typename... Ret, typename... Args>
Result<maybe_tuple<remove_all<Ret>...>, int> get_one(sqlite3 * db, StatusCode * status_out, std::string_view view, Args&&... args) {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(auto stmt, detail::prepare(db, status_out, view, std::forward<Args>(args)...));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt.handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_DONE: {
        LOG_SQLITE_ERROR("statement step finished prematurely");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step");
        RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
    }
    }

    maybe_tuple<remove_all<Ret>...> ret = detail::convert<Ret...>(stmt.handle);

    LOG_TRACE("successfully ran statement");

    return ok(std::move(ret));
}

template<typename... Ret, typename FCond, typename FIns, typename... Args>
Result<void, int> get_many_impl(sqlite3 * db, StatusCode * status_out, std::string_view view, FCond cond, FIns ins, Args&&... args) {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(auto stmt, detail::prepare(db, status_out, view, std::forward<Args>(args)...));

    size_t i = 0;

    while (cond(i)) {
        int rc = SQLITE_OK;
        switch (rc = sqlite3_step(stmt.handle)) {
        case SQLITE_DONE: {
            goto done;
        }
        case SQLITE_ROW: {
            ins(i++, std::move(detail::convert<Ret...>(stmt.handle)));
            break;
        }
        default: {
            LOG_SQLITE_ERROR("unknown step error");
            RETURN_STATUS_OUT(err(rc), StatusCode_InternalError);
        }
        }
    }

done:
    LOG_TRACE("successfully ran statement");

    return ok();
}

template<typename T, typename... Args>
Result<size_t, int> get_many_buffered(
    sqlite3 * db, StatusCode * status_out, std::string_view view, T * out_arr, size_t out_arr_size, Args&&... args
) {
    return get_many_impl<remove_all<T>>(
        db,
        status_out,
        view,
        [out_arr_size](int i) { return i < out_arr_size; },
        [out_arr](size_t i, std::tuple<remove_all<T>>&& val) { out_arr[i] = std::get<0>(val); },
        std::forward<Args>(args)...
    );
}

template<typename... Ret, typename... Args>
Result<std::vector<std::tuple<Ret...>>, int> get_many(sqlite3 * db, StatusCode * status_out, std::string_view view, Args&&... args) {
    std::vector<std::tuple<remove_all<Ret>...>> ret{};

    TRY(get_many_impl<remove_all<Ret>...>(
        db,
        status_out,
        view,
        [](int i) { return true; },
        [&ret](size_t i, std::tuple<remove_all<Ret>...>&& val) { ret.emplace_back(std::move(val)); },
        std::forward<Args>(args)...
    ));

    return ok(ret);
}

template<bool exclusive, typename F>
Result<void, int> transact(sqlite3 * db, StatusCode * status_out, F func) {
    if constexpr (exclusive) {
        TRY(db::exec(db, status_out, "BEGIN EXCLUSIVE TRANSACTION;"));
    } else {
        TRY(db::exec(db, status_out, "BEGIN TRANSACTION;"));
    }

    if (auto res = func(db); res.is_err()) {
        LOG_SQLITE_ERROR_RC("unable to run transaction", res.unwrap_error());
        auto _ = db::exec(db, status_out, "ROLLBACK TRANSACTION;");
        return res;
    }

    TRY(db::exec(db, status_out, "COMMIT TRANSACTION;"));

    return ok();
}

inline int changes(sqlite3 * db) {
    return sqlite3_changes(db);
}

}
