#pragma once


// local
#include <when.hpp>

// std
#include <optional>
#include <type_traits>
#include <utility>


namespace rp {
namespace detail {


template <typename ...Args>
struct S;


template <typename ...Args>
struct Valid : std::true_type {};


template <typename ...Args>
constexpr bool valid(Args&&...) { return true; }


template <typename F, typename AS, typename = void>
struct Callable : Callable<F, AS, When<true>> {};


template <typename F, typename AS, bool fallback>
struct Callable<F, AS, When<fallback>> : std::false_type {};


template <typename F, typename ...Args>
struct Callable<
        F,
        S<Args...>,
        When<Valid<decltype(
            std::declval<F>()(std::declval<Args>()...))>::value>>
    : std::true_type
{};


} // namespace detail


template <typename T>
struct Type {};


template <typename T>
constexpr Type<T> type_c{};


template <typename F, typename ...Args>
constexpr bool callable(F&&, Type<Args> const&...) noexcept {
    return detail::Callable<F, detail::S<Args...>>::value;
}


template <typename ...Args>
struct Overload : public Args... {
    using Args::operator()...;
};


template <typename ...Args>
Overload(Args...) -> Overload<Args...>;


template <typename T>
struct DependentFalse : std::false_type {};


///
/// Is `T` the `std::optional` type
///
template <typename T>
struct IsOptional : std::false_type {};


template <typename T>
struct IsOptional<std::optional<T>> : std::true_type {};


template <typename T>
constexpr auto isOptional(rp::Type<T>) {
    return IsOptional<std::decay_t<T>>::value;
}


} // namespace rp
