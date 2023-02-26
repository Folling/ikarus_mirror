#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <variant>

#include <util/base.hpp>
#include <util/structs/option.hpp>

#define CONCAT(a, b)       CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b

#define UNIQUE_NAME(base)  CONCAT(base, __COUNTER__)

#define TRY_IMPL(var_name, ...)  \
    auto var_name = __VA_ARGS__; \
    if (var_name.is_err()) {     \
        return var_name;         \
    }                            \
    [] {}()

#define TRY(...) TRY_IMPL(UNIQUE_NAME(result), __VA_ARGS__)

#define VTRY_IMPL(var_name, value, ...) \
    auto var_name = __VA_ARGS__;        \
    if (var_name.is_err()) {            \
        return var_name;                \
    }                                   \
    value = var_name.unwrap_value()

#define VTRY(value, ...) VTRY_IMPL(UNIQUE_NAME(result), value, __VA_ARGS__)

#define TRYRV_IMPL(var_name, ret, ...) \
    auto var_name = __VA_ARGS__;       \
    if (var_name.is_err()) {           \
        return ret;                    \
    }                                  \
    [] {}()

#define TRYRV(ret, ...) TRYRV_IMPL(UNIQUE_NAME(result), ret, __VA_ARGS__)

#define VTRYRV_IMPL(var_name, value, ret, ...) \
    auto var_name = __VA_ARGS__;               \
    if (var_name.is_err()) {                   \
        return ret;                            \
    }                                          \
    value = var_name.unwrap_value()

#define VTRYRV(value, ret, ...) VTRYRV_IMPL(UNIQUE_NAME(result), value, ret, __VA_ARGS__)

namespace detail {

template<typename T, bool>
struct ResultHelper {
    T value;
};

template<bool B>
struct ResultHelper<void, B> {};

struct OkResultCreationHelper {
    template<typename T>
        requires std::copy_constructible<T>
    constexpr auto operator()(T const& value) const noexcept -> ResultHelper<T, true> {
        return {value};
    }

    template<typename T>
        requires(std::move_constructible<T> && !std::is_lvalue_reference_v<T>)
    constexpr auto operator()(T&& value) const noexcept -> ResultHelper<T, true> {
        return {std::forward<T>(value)};
    }

    constexpr auto operator()() const noexcept -> ResultHelper<void, true> {
        return {};
    }
};

struct ErrResultCreationHelper {
    template<typename T>
        requires std::copy_constructible<T>
    constexpr auto operator()(T const& value) const noexcept -> ResultHelper<T, false> {
        return {value};
    }

    template<typename T>
        requires(std::move_constructible<T> && !std::is_lvalue_reference_v<T>)
    constexpr auto operator()(T&& value) const noexcept -> ResultHelper<T, false> {
        return {std::forward<T>(value)};
    }

    constexpr auto operator()() const noexcept -> ResultHelper<void, false> {
        return {};
    }
};

}

inline constexpr detail::OkResultCreationHelper ok{};
inline constexpr detail::ErrResultCreationHelper err{};

template<typename V, typename E>
class Result {
    typedef V value_type;
    typedef E error_type;

public:
    template<typename V2>
    Result(detail::ResultHelper<V2, true> helper)
        requires std::constructible_from<V, V2&&>
    {
        _has_error = false;
        new (data.data()) V{std::move(helper.value)};
    }

    template<typename E2>
    Result(detail::ResultHelper<E2, false> helper)
        requires std::constructible_from<E, E2&&>
    {
        _has_error = true;

        new (data.data()) E{std::move(helper.value)};
    }

    template<typename T, typename E2>
        requires std::constructible_from<E, E2>
    Result(Result<T, E2> const& other) {
        if (other.is_err()) {
            _has_error = true;
            new (data.data()) E2{other.unwrap_error()};

            return;
        }

        _has_error = false;

        // don't emplace otherwise - it doesn't really make sense to store a value
        // if we converted from a different result which differed in type
    }

public:
    Result<V, E> vput_into(V& value) {
        if (!is_ok()) {
            return *this;
        }

        value = unwrap_value();

        return *this;
    }

public:
    [[nodiscard]] bool is_ok() const {
        return !_has_error;
    }

    [[nodiscard]] bool is_err() const {
        return _has_error;
    }

    [[nodiscard]] V * handle_value() {
        return reinterpret_cast<V *>(data.data());
    }

    [[nodiscard]] V const * handle_value() const {
        return reinterpret_cast<V const *>(data.data());
    }

    [[nodiscard]] V unwrap_value()
        requires(std::is_move_constructible_v<V> && !std::is_copy_constructible_v<V>)
    {
        return std::move(*reinterpret_cast<V *>(data.data()));
    }

    [[nodiscard]] V unwrap_value() const {
        return *reinterpret_cast<V const *>(data.data());
    }

    [[nodiscard]] V unwrap_value_or(V const& substitute)
        requires(std::is_move_constructible_v<V> && !std::is_copy_constructible_v<V>)
    {
        if (is_ok()) {
            return std::move(*reinterpret_cast<V *>(data.data()));
        }

        return substitute;
    }

    [[nodiscard]] V unwrap_value_or(V const& substitute) const {
        if (is_ok()) {
            return *reinterpret_cast<V const *>(data.data());
        }

        return substitute;
    }

    template<typename F>
    [[nodiscard]] V unwrap_value_or_else(F generator)
        requires(std::is_move_constructible_v<V> && !std::is_copy_constructible_v<V>)
    {
        if (is_ok()) {
            return std::move(*reinterpret_cast<V *>(data.data()));
        }

        return generator(unwrap_error());
    }

    template<typename F>
    [[nodiscard]] V unwrap_value_or_else(F generator) const {
        if (is_ok()) {
            return *reinterpret_cast<V const *>(data.data());
        }

        return generator(unwrap_error());
    }

    template<typename F>
    Result<V, E> inspect_value(F func) const {
        if (is_ok()) {
            func(get_value_handle());
        }

        return *this;
    }

    template<typename F>
    Result<V, E> inspect_error(F func) const {
        if (is_err()) {
            func(get_error_handle());
        }

        return *this;
    }

    [[nodiscard]] V * get_value_handle() {
        return reinterpret_cast<V *>(data.data());
    }

    [[nodiscard]] V const * get_value_handle() const {
        return reinterpret_cast<V const *>(data.data());
    }

    [[nodiscard]] E * get_error_handle() {
        return reinterpret_cast<E *>(data.data());
    }

    [[nodiscard]] E const * get_error_handle() const {
        return reinterpret_cast<E const *>(data.data());
    }

    [[nodiscard]] E unwrap_error() const {
        return *reinterpret_cast<E const *>(data.data());
    }

    [[nodiscard]] Option<V> get_value() const {
        if (is_err()) {
            return none();
        }

        return some(unwrap_value());
    }

    [[nodiscard]] Option<E> get_error() const {
        if (is_ok()) {
            return none();
        }

        return some(unwrap_error());
    }

    template<typename T>
    [[nodiscard]] Result<T, E> change_type_if_err() {
        if (is_err()) {
            return err(unwrap_error());
        }

        return ok(T{});
    }

    template<typename F>
    [[nodiscard]] Result<std::result_of_t<F(V)>, E> map(F transformer) const {
        if (is_err()) {
            return *this;
        }

        return ok(transformer(unwrap_value()));
    }

    template<typename F>
    [[nodiscard]] std::result_of_t<F(V)> map_or(std::result_of_t<F(V)> const& default_value, F transformer) const {
        if (is_err()) {
            return default_value;
        }

        return transformer(unwrap_value());
    }

    template<typename F1, typename F2>
        requires(std::is_same_v<std::result_of_t<F1(V)>, std::result_of_t<F2(V)>>)
    [[nodiscard]] Result<std::result_of_t<F1(V)>, E> map_or_else(F1 error_transformer, F2 value_transformer) const {
        if (is_err()) {
            return err(error_transformer(unwrap_error()));
        }

        return ok(value_transformer(get_value()));
    }

    template<typename T>
    [[nodiscard]] Result<T, E> convert() {
        if (is_err()) {
            return *this;
        }

        return ok(T{unwrap_value()});
    }

    template<typename T>
    [[nodiscard]] T convert_or(T const& default_value) {
        if (is_err()) {
            return default_value;
        }

        return T{unwrap_value()};
    }

    template<typename E2>
    [[nodiscard]] Result<V, E2> map_err(E2 other_error) {
        if (is_err()) {
            return err(other_error);
        }

        return ok(unwrap_value());
    }

    template<typename E2, typename F>
    [[nodiscard]] Result<V, E2> map_err_with(F transformer) {
        if (is_err()) {
            return err(transformer(unwrap_error()));
        }

        return ok(unwrap_value());
    }

    template<typename T>
    [[nodiscard]] Result<T, E> combine(Result<T, E> const& res) const {
        if (is_err() || res.is_err()) {
            return err(unwrap_error());
        }

        return res;
    }

    template<typename T, typename F>
    [[nodiscard]] Result<T, E> combine_then(F const& transformer) const {
        if (is_err()) {
            return err(unwrap_error());
        }

        return transformer(unwrap_value());
    }

    [[nodiscard]] Result<V, E> _or(Result<V, E> const& comparate) const {
        if (is_ok()) {
            return ok(unwrap_value());
        }

        if (!comparate.is_err()) {
            return ok(comparate.unwrap_value());
        }

        return err(comparate.unwrap_error());
    }

    template<typename E2, typename F>
    [[nodiscard]] Result<V, E2> _or_else(F transformer) {
        if (is_err()) {
            return transformer(unwrap_error());
        }

        return *this;
    }

    Result<V, E> print_err(std::string_view msg) {
        if (is_err()) {
            LOG_ERROR(msg, unwrap_error());
        }

        return *this;
    }

public:
    [[nodiscard]] explicit operator bool() {
        return is_ok();
    }

private:
    std::array<u8, std::max(sizeof(V), sizeof(E))> data;
    bool _has_error;
};

template<typename E>
class Result<void, E> {
public:
    Result(detail::ResultHelper<void, true>) {
        _has_error = false;
    }

    template<typename E2>
    Result(detail::ResultHelper<E2, false> helper)
        requires std::constructible_from<E, E2&&>
    {
        _has_error = true;
        new (_data.data()) E{std::move(helper.value)};
    }

    template<typename T, typename E2>
        requires std::constructible_from<E, E2>
    Result(Result<T, E2> const& other) {
        if (other.is_err()) {
            _has_error = true;
            new (_data.data()) E2{other.unwrap_error()};

            return;
        }

        _has_error = false;

        // don't emplace otherwise - it doesn't really make sense to store a value
        // if we converted from a different result which differed in type
    }

public:
    template<typename F>
    Result<void, E> inspect_error(F func) const {
        if (is_err()) {
            func(get_error_handle());
        }

        return *this;
    }

    [[nodiscard]] E * get_error_handle() {
        return reinterpret_cast<E *>(_data.data());
    }

    [[nodiscard]] E const * get_error_handle() const {
        return reinterpret_cast<E const *>(_data.data());
    }

    [[nodiscard]] bool is_ok() const {
        return !_has_error;
    }

    [[nodiscard]] bool is_err() const {
        return _has_error;
    }

    [[nodiscard]] E unwrap_error() const {
        return *reinterpret_cast<E const *>(_data.data());
    }

    [[nodiscard]] Option<E> get_error() const {
        if (!is_err()) {
            return none();
        }

        return some(unwrap_error());
    }

    template<typename F>
    [[nodiscard]] Result<void, std::result_of_t<F(E)>> map_err(F transformer) {
        if (is_err()) {
            return err(transformer(unwrap_error()));
        }

        return ok();
    }

    template<typename T>
    [[nodiscard]] Result<T, E> combine(Result<T, E> const& res) const {
        if (is_err() || res.is_err()) {
            return err(unwrap_error());
        }

        return res;
    }

    template<typename E2, typename F>
    [[nodiscard]] Result<void, E2> _or_else(F transformer) {
        if (is_err()) {
            return transformer(unwrap_error());
        }

        return *this;
    }

private:
    std::array<u8, sizeof(E)> _data;
    bool _has_error;
};

template<typename V, typename E>
struct fmt::formatter<Result<V, E>> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Result<V, E> const& res, fmt::format_context& ctx) const {
        if (res.is_ok()) {
            return fmt::format_to(ctx.out(), "Ok({})", *res.get_value_handle());
        } else {
            return fmt::format_to(ctx.out(), "Err({})", *res.get_error_handle());
        }
    }
};

template<typename E>
struct fmt::formatter<Result<void, E>> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Result<void, E> const& res, fmt::format_context& ctx) const {
        if (res.is_ok()) {
            return fmt::format_to(ctx.out(), "Ok()");
        } else {
            return fmt::format_to(ctx.out(), "Err({})", *res.inspect_error());
        }
    }
};