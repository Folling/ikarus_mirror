#pragma once

#include "coded/util/base.hpp"

template<typename V, typename E>
class Result;

namespace detail {

template<typename T, bool>
struct OptionHelper {
    T value;
};

template<typename T>
struct OptionHelper<T, false> {};

template<bool B>
struct OptionHelper<void, B> {};

template<>
struct OptionHelper<void, false> {};

struct SomeOptionCreationHelper {
    template<typename T>
        requires std::copy_constructible<T>
    constexpr auto operator()(T const& value) const noexcept -> OptionHelper<T, true> {
        return {value};
    }

    template<typename T>
        requires(std::move_constructible<T> && !std::is_lvalue_reference_v<T>)
    constexpr auto operator()(T&& value) const noexcept -> OptionHelper<T, true> {
        return {std::forward<T>(value)};
    }

    constexpr auto operator()() const noexcept -> OptionHelper<void, true> {
        return {};
    }
};

struct NoneOptionCreationHelper {
    template<typename T>
    constexpr auto operator()() const noexcept -> OptionHelper<T, false> {
        return {};
    }

    constexpr auto operator()() const noexcept -> OptionHelper<void, false> {
        return {};
    }
};

}

inline constexpr detail::SomeOptionCreationHelper some{};
inline constexpr detail::NoneOptionCreationHelper none{};

template<typename T>
    requires(!std::is_void_v<T>)
class Option {
public:
    template<typename T2>
        requires std::constructible_from<T, T2&&>
    constexpr Option(detail::OptionHelper<T2, true> data):
        loaded{true},
        data{} {
        new (this->data.data()) T{std::move(data.value)};
    }

    constexpr Option(detail::OptionHelper<void, false>):
        loaded{false},
        data{} {}

    constexpr Option():
        loaded{false},
        data{} {}

public:
    [[nodiscard]] constexpr bool is_some() const {
        return loaded;
    }

    [[nodiscard]] constexpr bool is_none() const {
        return !loaded;
    }

    [[nodiscard]] constexpr T * inspect() {
        return reinterpret_cast<T *>(data.data());
    }

    [[nodiscard]] constexpr T const * inspect() const {
        return reinterpret_cast<T const *>(data.data());
    }

    [[nodiscard]] constexpr T unwrap()
        requires(std::is_move_constructible_v<T> && !std::is_copy_constructible_v<T>)
    {
        return std::move(*reinterpret_cast<T *>(data.data()));
    }

    [[nodiscard]] constexpr T unwrap() const {
        return *reinterpret_cast<T const *>(data.data());
    }

    [[nodiscard]] constexpr T unwrap_or(T const& default_value)
        requires(std::is_move_constructible_v<T> && !std::is_copy_constructible_v<T>)
    {
        if (is_none()) {
            return default_value;
        }

        return std::move(*reinterpret_cast<T *>(data.data()));
    }

    [[nodiscard]] constexpr T unwrap_or(T const& default_value) const {
        if (is_none()) {
            return default_value;
        }

        return *reinterpret_cast<T const *>(data.data());
    }

    template<typename F>
    [[nodiscard]] constexpr T unwrap_or_else(F generator)
        requires(std::is_move_constructible_v<T> && !std::is_copy_constructible_v<T>)
    {
        if (is_none()) {
            return generator();
        }

        return std::move(*reinterpret_cast<T *>(data.data()));
    }

    template<typename F>
    [[nodiscard]] constexpr T unwrap_or_else(F generator) const {
        if (is_none()) {
            return generator();
        }

        return *reinterpret_cast<T const *>(data.data());
    }

public:
    constexpr void insert() {
        new (data.data()) T{};
    }

    constexpr void insert(T value) {
        loaded = true;
        new (data.data()) T{std::move(value)};
    }

    constexpr void empty_insert(T value) {
        if (is_none()) {
            return;
        }

        loaded = true;
        new (data.data()) T{std::move(value)};
    }

    template<typename F>
    constexpr void insert_delayed(F generator) {
        loaded = true;
        new (data.data()) T{std::move(generator())};
    }

    template<typename F>
    constexpr void empty_insert_delayed(F generator) {
        if (is_none()) {
            return;
        }

        loaded = true;
        new (data.data()) T{std::move(generator())};
    }

public:
    template<typename T2>
    [[nodiscard]] constexpr Option<T2> _and(Option<T2> const& other) const {
        if (is_none()) {
            return none();
        }

        return other;
    }

    template<typename F>
    [[nodiscard]] constexpr Option<std::result_of_t<F>> _and_then(F const& transformer) const {
        if (is_none()) {
            return none();
        }

        return transformer();
    }

    [[nodiscard]] constexpr Option<T> _or(Option<T> const& comparate) const {
        if (!is_none()) {
            return *this;
        }

        return comparate;
    }

    template<typename F>
    [[nodiscard]] constexpr Option<T> _or_else(F generator) const {
        if (is_none()) {
            return generator();
        }

        return *this;
    }

    template<typename F>
    [[nodiscard]] constexpr Option<std::result_of_t<F(T)>> map(F generator) const {
        if (is_none()) {
            return none();
        }

        return some(generator(unwrap()));
    }

    template<typename F>
    [[nodiscard]] constexpr std::result_of_t<F(T)> map_or(F generator, std::result_of_t<F(T)> default_value) const {
        if (is_none()) {
            return default_value;
        }

        return generator(unwrap());
    }

    template<typename F, typename F2>
        requires(std::is_same_v<std::result_of_t<F(T)>, std::result_of_t<F>>)
    [[nodiscard]] constexpr std::result_of_t<F(T)> map_or_else(F generator, F2 default_value_generator) const {
        if (is_none()) {
            return default_value_generator();
        }

        return generator(unwrap());
    }

    void clear() {
        loaded = false;
    }

    template<std::predicate P>
    [[nodiscard]] constexpr Option<std::result_of_t<P>> filter(P predicate) const {
        if (is_none()) {
            return none();
        }

        auto value = unwrap();

        if (!predicate(value)) {
            return none();
        }

        return some(value);
    }

    template<std::predicate P>
    constexpr void filter_inplace(P predicate) {
        loaded = !is_none() && predicate(unwrap());
    }

public:
    template<typename E>
    [[nodiscard]] constexpr Result<T, E> ok_or(E const& e) const {
        if (is_some()) {
            return ok(unwrap());
        } else {
            return err(e);
        }
    }

    template<typename F>
    [[nodiscard]] constexpr Result<T, std::invoke_result_t<F>> ok_or_else(F generator) const {
        if (is_some()) {
            return ok(unwrap());
        } else {
            return err(generator());
        }
    }

public:
    template<typename T2>
    [[nodiscard]] constexpr bool operator==(T2 const& comparate) const {
        return is_some() && *inspect() == comparate;
    }

    template<typename T2>
    [[nodiscard]] constexpr bool operator!=(T2 const& comparate) const {
        return is_none() || *inspect() != comparate;
    }

    template<typename T2>
    [[nodiscard]] constexpr bool operator<(T2 const& comparate) const {
        return is_some() && *inspect() < comparate;
    }

    template<typename T2>
    [[nodiscard]] constexpr bool operator==(Option<T2> const& comparate) const {
        return (is_some() && comparate.is_some() && *inspect() == *comparate.inspect()) || (is_none() && comparate.is_none());
    }

    template<typename T2>
    [[nodiscard]] constexpr bool operator!=(Option<T2> const& comparate) const {
        return (is_some() && comparate.is_some() && *inspect() == *comparate.inspect()) || (loaded != comparate.loaded);
    }

    template<typename T2>
    [[nodiscard]] constexpr bool operator<(Option<T2> const& comparate) const {
        return is_some() && comparate.is_some() && *inspect() < *comparate.inspect();
    }

private:
    bool loaded;
    std::array<byte, sizeof(T)> data;
};

template<typename T>
struct fmt::formatter<Option<T>> {
    template<typename FormatParseContext>
    auto parse(FormatParseContext& pc) {
        return pc.end();
    }

    auto format(Option<T> const& opt, fmt::format_context& ctx) const {
        if (opt.is_some()) {
            return fmt::format_to(ctx.out(), "Some({})", *opt.inspect());
        } else {
            return fmt::format_to(ctx.out(), "None");
        }
    }
};