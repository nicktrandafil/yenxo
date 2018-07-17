#pragma once


// local
#include <when.hpp>

// std
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


template <typename F, typename ...Args>
constexpr bool callable(F&&, Type<Args>&&...) noexcept {
    return detail::Callable<F, detail::S<Args...>>::value;
}


template <typename ...Args>
struct Overload : public Args... {
    using Args::operator()...;
};


template <typename ...Args>
Overload(Args...) -> Overload<Args...>;


} // namespace rp
