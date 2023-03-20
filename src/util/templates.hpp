#pragma once

#include <limits>
#include <memory>
#include <type_traits>

#include <util/types.hpp>

template<typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overload(Ts...) -> overload<Ts...>;

template<typename... Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

template<typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;

template<int I, int I2, int... Rest>
struct index_of {
    static constexpr int const value = (I == I2 ? 0 : (index_of<I, Rest...>::value >= 0 ? 1 + index_of<I, Rest...>::value : -1));
};

template<int I, int I2>
struct index_of<I, I2> {
    static constexpr int const value = I == I2 ? 0 : -1;
};

template<int I, int... Is>
constexpr auto index_of_v = index_of<I, Is...>::value;

template<typename T, typename T2, typename... Rest>
struct index_of_type {
    static constexpr int const value =
        (std::is_same_v<T, T2> ? 0 : (index_of_type<T, Rest...>::value >= 0 ? 1 + index_of_type<T, Rest...>::value : -1));
};

template<typename T, typename T2>
struct index_of_type<T, T2> {
    static constexpr int const value = std::is_same_v<T, T2> ? 0 : -1;
};

template<typename T, typename... Ts>
constexpr auto index_of_type_v = index_of_type<T, Ts...>::value;

template<typename T, typename... Ts>
struct any_of {
    static constexpr bool const value = index_of_type<T, Ts...>::value != -1;
};

template<typename T, typename... Ts>
constexpr auto any_of_v = any_of<T, Ts...>::value;

template<typename T, typename... Ts>
struct none_of {
    static constexpr bool const value = index_of_type<T, Ts...>::value == -1;
};

template<typename T, typename... Ts>
constexpr auto none_of_v = none_of<T, Ts...>::value;

template<typename T>
struct delayed_false : std::false_type {};

struct do_not_use;

template<>
struct delayed_false<do_not_use> : std::true_type {};

template<typename T>
constexpr auto delayed_false_v = delayed_false<T>::value;

template<typename F, typename... Ts, std::size_t... Is>
void for_each_in_param_pack_impl(F func, Ts&&... args, std::index_sequence<Is...>) {
    (func.template operator()<Is>(args), ...);
}

template<typename F, typename... Ts>
void for_each_in_param_pack(F func, Ts&&... args) {
    for_each_in_param_pack_impl<F, Ts...>(func, std::forward<Ts>(args)..., std::make_index_sequence<sizeof...(Ts)>());
}

template<typename From, typename To>
struct type_transformer {
    typedef To type;
};

template<typename From, typename To>
using type_transformer_t = typename type_transformer<From, To>::type;

template<u64 I, typename... Ts>
struct nth_type;

template<u64 I, typename T, typename... Ts>
struct nth_type<I, T, Ts...> {
    typedef typename nth_type<I - 1, Ts...>::type type;
};

template<typename T, typename... Ts>
struct nth_type<0, T, Ts...> {
    typedef T type;
};

template<u64 I, typename... Ts>
struct nth_offset;

template<u64 I, typename T, typename... Ts>
    requires(I > 0)
struct nth_offset<I, T, Ts...> {
    constexpr static auto value = nth_offset<I - 1, Ts...>::value + sizeof(T);
};

template<typename... Ts>
struct nth_offset<0, Ts...> {
    constexpr static auto value = 0;
};

template<u64 I, typename... Ts>
using nth_type_t = typename nth_type<I, Ts...>::type;

template<u64 I, typename... Ts>
inline constexpr u64 nth_offset_v = nth_offset<I, Ts...>::value;

template<typename... Ts>
using first_type_t = typename nth_type<0, Ts...>::type;

template<typename... Ts>
using last_type_t = typename nth_type<sizeof...(Ts) - 1, Ts...>::type;

template<typename... Ts>
constexpr auto last_param(Ts&&... args) {
    void * arg = nullptr;
    ((arg = &args), ...);
    return *reinterpret_cast<last_type_t<Ts...> *>(arg);
}

template<typename F, typename T, typename... Rest>
constexpr auto apply_to_first_param(F func, T const& first, Rest const&...) {
    return func(first);
}

template<typename T, typename... Ts>
constexpr std::size_t type_index_v() {
    std::size_t r = 0;
    auto test = [&](bool b) {
        if (!b) {
            ++r;
        }
        return b;
    };
    (test(std::is_same_v<T, Ts>) || ...);
    return r;
}

template<typename T>
using remove_all = std::remove_cvref_t<T>;
