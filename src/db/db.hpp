#pragma once

#include "error.hpp"

#include <sqlite3.h>

#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <db/sqlitehelper.hpp>
#include <util/logger.hpp>
#include <util/structs/result.hpp>
#include <util/templates.hpp>

namespace db {

template<typename... Args>
Result<void, int> exec(sqlite3 * db, std::string_view view, Args&&... args) {
    LOG_TRACE("executing statement:\n{}", view);

    VTRY(stmt, detail::prepare(db, view, std::forward<Args>(args)...));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt.handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step");
        return err(rc);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step");
        return err(rc);
    }
    case SQLITE_DONE: [[fallthrough]];
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step");
        return err(rc);
    }
    }

    LOG_TRACE("successfully executed statement");

    return ok();
}

template<typename... Args>
Result<void, int> exec_log(sqlite3 * db, std::string_view msg, std::string_view view, Args&&... args) {
    auto res = exec<Args...>(db, view, std::forward<Args>(args)...);

    if (res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(msg, res.unwrap_error());
    }

    return res;
}

template<typename... Ret, typename... Args>
Result<std::tuple<remove_all<Ret>...>, int> get_one_plural(sqlite3 * db, std::string_view view, Args&&... args) {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(stmt, detail::prepare(db, view, std::forward<Args>(args)...));

    int rc = SQLITE_OK;

    switch (rc = sqlite3_step(stmt.handle)) {
    case SQLITE_ERROR: {
        LOG_SQLITE_ERROR("error reported during statement step");
        return err(rc);
    }
    case SQLITE_MISUSE: {
        LOG_SQLITE_ERROR("misuse reported during statement step");
        return err(rc);
    }
    case SQLITE_DONE: {
        LOG_SQLITE_ERROR("statement step finished prematurely");
        return err(rc);
    }
    case SQLITE_ROW: {
        break;
    }
    default: {
        LOG_SQLITE_ERROR("unknown result reported during statement step");
        return err(rc);
    }
    }

    std::tuple<remove_all<Ret>...> ret = detail::convert<Ret...>(stmt.handle);

    LOG_TRACE("successfully ran statement");

    return ok(std::move(ret));
}

template<typename... Ret, typename... Args>
Result<std::tuple<remove_all<Ret>...>, int> get_one_plural_log(sqlite3 * db, std::string_view msg, std::string_view view, Args&&... args) {
    auto res = get_one_plural<Ret...>(db, view, std::forward<Args>(args)...);

    if (res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(msg, res.unwrap_error());
    }

    return res;
}

template<typename T, typename... Args>
Result<remove_all<T>, int> get_one(sqlite3 * db, std::string_view view, Args&&... args) {
    return get_one_plural<T>(db, view, std::forward<Args>(args)...).map([](auto const& v) { return std::get<0>(v); });
}

template<typename T, typename... Args>
Result<remove_all<T>, int> get_one_log(sqlite3 * db, std::string_view msg, std::string_view view, Args&&... args) {
    auto res = get_one<T>(db, view, std::forward<Args>(args)...);

    if (res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(msg, res.unwrap_error());
    }

    return res;
}

template<typename... Ret, typename FCond, typename FIns, typename... Args>
Result<void, int> get_many_impl(sqlite3 * db, std::string_view view, FCond cond, FIns ins, Args&&... args) {
    LOG_TRACE("executing statement with expected return:\n{}", view);

    VTRY(stmt, detail::prepare(db, view, std::forward<Args>(args)...));

    size_t i = 0;

    while (cond(i)) {
        int rc = SQLITE_OK;
        switch (rc = sqlite3_step(stmt.handle)) {
        case SQLITE_DONE: {
            goto done;
        }
        case SQLITE_ROW: {
            ins(i++, std::move(detail::convert_many<Ret...>(stmt.handle)));
            break;
        }
        default: {
            LOG_SQLITE_ERROR("unknown step error");
            return err(rc);
        }
        }
    }

done:
    LOG_TRACE("successfully ran statement");

    return ok();
}

template<typename T, typename... Args>
Result<void, int> get_many_buffered(sqlite3 * db, std::string_view view, T * out_arr, size_t out_arr_size, Args&&... args) {
    return get_many_impl<remove_all<T>>(
        db,
        view,
        [out_arr_size](int i) { return i < out_arr_size; },
        [out_arr](size_t i, std::tuple<remove_all<T>>&& val) { out_arr[i] = std::get<0>(val); },
        std::forward<Args>(args)...
    );
}

template<typename T, typename... Args>
Result<void, int> get_many_buffered_log(
    sqlite3 * db, std::string_view msg, std::string_view view, T * out_arr, size_t out_arr_size, Args&&... args
) {
    auto res = get_many_buffered<T>(db, view, out_arr, out_arr_size, std::forward<Args>(args)...);

    if (res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(msg, res.unwrap_error());
    }

    return res;
}

template<typename... Ret, typename... Args>
Result<std::vector<std::tuple<Ret...>>, int> get_many_plural(sqlite3 * db, std::string_view view, Args&&... args) {
    std::vector<std::tuple<remove_all<Ret>...>> ret{};

    TRY(get_many_impl<remove_all<Ret>...>(
        db,
        view,
        [](int i) { return true; },
        [&ret](size_t i, std::tuple<remove_all<Ret>...>&& val) { ret.emplace_back(std::move(val)); },
        std::forward<Args>(args)...
    ));

    return ok(ret);
}

template<typename T, typename... Args>
Result<remove_all<T>, int> get_many_plural_log(sqlite3 * db, std::string_view msg, std::string_view view, Args&&... args) {
    auto res = get_many_plural<T>(db, view, std::forward<Args>(args)...);

    if (res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(msg, res.unwrap_error());
    }

    return res;
}

template<bool exclusive, typename F>
Result<void, int> transact(sqlite3 * db, std::string_view err_msg, F func) {
    if constexpr (exclusive) {
        db::exec(db, "BEGIN EXCLUSIVE TRANSACTION;");
    } else {
        db::exec(db, "BEGIN TRANSACTION;");
    }

    if (auto res = func(db); res.is_err()) {
        LOG_SQLITE_ERROR_RC_RT(err_msg, res.unwrap_error());
        auto _ = db::exec(db, "ROLLBACK TRANSACTION;");
        return res;
    }

    db::exec(db, "COMMIT TRANSACTION;");

    return ok();
}

}
