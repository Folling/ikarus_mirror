#pragma once

#include <sqlite3.h>

#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include <fmt/format.h>

#include <status.hpp>
#include <util/logger.hpp>
#include <util/structs/result.hpp>
#include <util/templates.hpp>

namespace db {

struct RawString {
    uint64_t size;
    char const * data;
};

namespace detail {

template<typename T>
struct SQLiteHelper;

template<typename I>
    requires std::is_integral_v<I>
struct SQLiteHelper<I> {
    static I convert(sqlite3_stmt * stmt, std::size_t idx) {
        if constexpr (sizeof(I) <= sizeof(int)) {
            return sqlite3_column_int(stmt, static_cast<int>(idx));
        } else if constexpr (sizeof(I) <= sizeof(sqlite3_int64)) {
            // potential problems with 64-bit unsigned integers such as for IDs, but there's no nice way around it
            return static_cast<I>(sqlite3_column_int64(stmt, static_cast<int>(idx)));
        } else {
            static_assert(tmpl::delayed_false_v<I>, "cannot fit integer type as bound parameter to SQLite");
        }
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, I value) {
        if constexpr (sizeof(I) <= sizeof(int)) {
            return sqlite3_bind_int(stmt, static_cast<int>(idx), static_cast<int>(value));
        } else if constexpr (sizeof(I) <= sizeof(sqlite3_int64)) {
            // potential problems with 64-bit unsigned integers such as for IDs, but there's no nice way around it
            return sqlite3_bind_int64(stmt, static_cast<int>(idx), static_cast<sqlite3_int64>(value));
        } else {
            static_assert(tmpl::delayed_false_v<I>, "cannot fit integer type as bound parameter to SQLite");
        }
    }
};

template<typename F>
    requires std::is_floating_point_v<F>
struct SQLiteHelper<F> {
    static F convert(sqlite3_stmt * stmt, std::size_t idx) {
        if constexpr (sizeof(F) <= sizeof(double)) {
            return sqlite3_column_double(stmt, static_cast<int>(idx));
        } else {
            static_assert(tmpl::delayed_false<F>{}, "cannot fit floating point type as bound parameter to SQLite");
        }
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, F value) {
        if constexpr (sizeof(F) <= sizeof(double)) {
            return sqlite3_bind_double(stmt, static_cast<int>(idx), static_cast<double>(value));
        } else {
            static_assert(tmpl::delayed_false<F>{}, "cannot fit floating point type as bound parameter to SQLite");
        }
    }
};

template<>
struct SQLiteHelper<RawString> {
    static RawString convert(sqlite3_stmt * stmt, std::size_t idx) {
        RawString ret{};
        // order is important, text must be called first
        ret.data = reinterpret_cast<char const *>(sqlite3_column_text(stmt, static_cast<int>(idx)));
        ret.size = sqlite3_column_bytes(stmt, static_cast<int>(idx));

        return ret;
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, RawString const& value) {
        // TODO, remove SQLITE_TRANSIENT for better performance
        return sqlite3_bind_text(stmt, static_cast<int>(idx), value.data, value.size, SQLITE_TRANSIENT);
    }
};

template<typename T>
    requires std::is_convertible_v<T, std::string_view>
struct SQLiteHelper<T> {
    static std::string convert(sqlite3_stmt * stmt, std::size_t idx) {
        auto text = sqlite3_column_text(stmt, static_cast<int>(idx));
        if (text == nullptr) {
            return std::string{};
        } else {
            return std::string{reinterpret_cast<char const *>(text)};
        }
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, T const& value) {
        std::string_view view = value;
        // TODO, remove SQLITE_TRANSIENT for better performance
        return sqlite3_bind_text(stmt, static_cast<int>(idx), view.data(), static_cast<int>(view.size()), SQLITE_TRANSIENT);
    }
};

template<typename T>
struct SQLiteHelper<Option<T>> {
    static Option<T> convert(sqlite3_stmt * stmt, std::size_t idx) {
        if (sqlite3_column_type(stmt, static_cast<int>(idx)) == SQLITE_NULL) {
            return none();
        } else {
            return some(SQLiteHelper<T>::convert(stmt, idx));
        }
    }

    static int bind(sqlite3_stmt * stmt, std::size_t idx, Option<T> const& value) {
        if (value.is_some()) {
            return SQLiteHelper<T>::bind(stmt, idx, value.unwrap());
        } else {
            return sqlite3_bind_null(stmt, static_cast<int>(idx));
        }
    }
};

template<typename... Args, std::size_t... Is>
int bind_impl(sqlite3_stmt * stmt, Args&&... args, std::index_sequence<Is...>) {
    int res = SQLITE_OK;

    auto lambda = [&res]<typename T>(sqlite3_stmt * stmt, T&& arg, std::size_t idx) {
        if (res != SQLITE_OK) {
            // the best equivalent to an early return
            return;
        }

        // + 1 because they start at 1
        res = SQLiteHelper<tmpl::remove_all<T>>::bind(stmt, idx + 1, arg);
    };

    (lambda(stmt, args, Is), ...);

    return res;
}

template<typename... Args>
int bind(sqlite3_stmt * stmt, Args&&... args) {
    return bind_impl<Args...>(stmt, std::forward<Args>(args)..., std::make_index_sequence<sizeof...(Args)>());
}

template<typename... Ret, std::size_t... Is>
std::tuple<Ret...> convert_many_impl(sqlite3_stmt * stmt, std::index_sequence<Is...>) {
    return std::make_tuple(std::forward<Ret>(SQLiteHelper<Ret>::convert(stmt, Is))...);
}

template<typename... Ret>
tmpl::maybe_tuple<Ret...> convert(sqlite3_stmt * stmt) {
    return tmpl::maybe_singularise(convert_many_impl<Ret...>(stmt, std::make_index_sequence<sizeof...(Ret)>()));
}

}

}
