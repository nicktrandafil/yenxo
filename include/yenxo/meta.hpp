/*
  MIT License

  Copyright (c) 2018 Nicolai Trandafil

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <yenxo/config.hpp>
#include <yenxo/when.hpp>

#if YENXO_ENABLE_TYPE_SAFE
#include <type_safe/constrained_type.hpp>
#include <type_safe/strong_typedef.hpp>
#include <type_safe/types.hpp>
#endif

#include <boost/hana/any_of.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

#include <array>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace yenxo {

/// Sequence of types
/// \ingroup group-meta
template <typename... Args>
struct S {
    template <typename T>
    using push_front = S<T, Args...>;

    template <template <class...> class T>
    using rebind = T<Args...>;

    template <template <class...> class T>
    using rebind_t = T<boost::hana::type<Args>...>;

    template <typename Key>
    constexpr static bool convertible() {
        return boost::hana::any_of(boost::hana::tuple_t<Args...>, [](auto x) {
            return std::is_convertible_v<Key, typename decltype(x)::type>;
        });
    }

    template <typename Key>
    constexpr static bool anyOf() {
        return boost::hana::type_c<Key> ^ boost::hana::in ^ boost::hana::tuple_t<Args...>;
    }
};

namespace detail {

/// A helper in SFINAE context
/// \ingroup group-details
///
/// Evaluates to `std::true_type` if `Args` are valid in SFINAE context.
template <typename... Args>
struct Valid : std::true_type {};

} // namespace detail

/// `std::variant` visitor function objects overload
/// \ingroup group-meta
///
/// In the code below
/// \code
///     Overload{[](int) {}, [](double) {}};
/// \endcode
/// the expression returns a function object that has
/// two overloads of `operator()`.
template <typename... Args>
struct Overload : public Args... {
    using Args::operator()...;
};

template <typename... Args>
Overload(Args...) -> Overload<Args...>;

/// A helper in `static_assert` context
/// \ingroup group-meta
///
/// Always Evaluates to `std::false_type`. The type is
/// templated in order to delay the `static_assert` until
/// the function is actually instantiated by providing a type
/// parameter.
template <typename>
struct DependentFalse : std::false_type {};

/// A helper in `static_assert` context
/// \ingroup group-meta
///
/// Same as DependentFalse but just for value template arguments.
template <auto...>
struct DependentFalseV : std::false_type {};

namespace detail {

template <typename T>
struct IsOptionalImpl : std::false_type {};

template <typename T>
struct IsOptionalImpl<std::optional<T>> : std::true_type {};

} // namespace detail

/// Test if `T` is `std::optional`
/// \ingroup group-meta
template <typename T>
constexpr auto isOptional(boost::hana::basic_type<T>) {
    return detail::IsOptionalImpl<std::remove_cv_t<std::remove_reference_t<T>>>::value;
}

#ifndef YENXO_DOXYGEN_INVOKED
namespace detail {

template <typename T, typename = void>
struct IsIterableImpl : IsIterableImpl<T, When<true>> {};

template <typename T, bool condition>
struct IsIterableImpl<T, When<condition>> : std::false_type {};

template <typename T>
struct IsIterableImpl<
        T,
        When<detail::Valid<decltype(
                // begin/end and operator !=
                static_cast<void>(begin(std::declval<T&>()) != end(std::declval<T&>())),

                // operator ++
                ++std::declval<decltype(begin(std::declval<T&>()))&>(),

                // operator*
                *begin(std::declval<T&>()))>::value>>
        : std::true_type

{};

} // namespace detail
#endif

/// Test if `T` is an iterable type
/// \ingroup group-meta
template <typename T>
constexpr auto isIterable(boost::hana::basic_type<T> const&) {
    return detail::IsIterableImpl<T>::value;
}

template <class T>
using IsIterable = detail::IsIterableImpl<T>;

#ifndef YENXO_DOXYGEN_INVOKED
namespace detail {

template <typename T, typename = void>
struct IsStringImpl : IsStringImpl<T, When<true>> {};

template <typename T, bool condition>
struct IsStringImpl<T, When<condition>> : std::false_type {};

template <typename T>
struct IsStringImpl<
        T,
        When<std::is_convertible_v<T,
                                   std::basic_string<typename T::value_type,
                                                     typename T::traits_type,
                                                     typename T::allocator_type>>>>
        : std::true_type {};

} // namespace detail
#endif

template <typename T>
constexpr auto isString(boost::hana::basic_type<T> const&) {
    return detail::IsStringImpl<T>::value;
}

template <class T>
using IsString = detail::IsStringImpl<T>;

namespace detail {

template <typename T>
struct IsPair : std::false_type {};

template <typename First, typename Second>
struct IsPair<std::pair<First, Second>> : std::true_type {};

} // namespace detail

/// Test if `T` is `std::pair`
/// \ingroup group-meta
template <typename T>
constexpr auto isPair(boost::hana::basic_type<T> const&) {
    return detail::IsPair<T>::value;
}

template <class T>
using IsPair = detail::IsPair<T>;

/// Test if `T` is a container
/// \ingroup group-meta
template <typename T>
constexpr auto isContainer(boost::hana::basic_type<T> const& x) {
    return isIterable(x) && !isString(x);
}

template <class T>
using IsContainer = std::conjunction<IsIterable<T>, std::negation<IsString<T>>>;

/// Test if `T` is a `std::pair` with `std::string` key
/// \ingroup group-meta
template <typename T>
constexpr auto isKeyValue(boost::hana::basic_type<T> const&) {
    if constexpr (isPair(boost::hana::type_c<T>)) {
        return isString(boost::hana::type_c<typename T::first_type>);
    } else {
        return false;
    }
}

template <class T>
using IsKeyValue = std::conjunction<IsPair<T>, IsString<typename T::first_type>>;

/// Test if `type` is has `push_back` method
/// \ingroup group-meta
constexpr auto hasPushBack = boost::hana::is_valid(
        [](auto type)
                -> decltype((void)boost::hana::traits::declval(type).push_back(
                        std::declval<typename decltype(type)::type::value_type>())) {});

/// Test if `type` is has `emplace` method
/// \ingroup group-meta
constexpr auto hasEmplace = boost::hana::is_valid(
        [](auto x) -> decltype((void)boost::hana::traits::declval(x).emplace(
                           std::declval<typename decltype(x)::type::value_type>())) {});

#if YENXO_ENABLE_TYPE_SAFE
/// Tests if type is `type_safe::strong_typedef`
/// \ingroup group-meta
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto strongTypeDef = [](auto type) { return isStrongTypeDef(type); };
#else
template <typename T>
struct StrongTypeDefImpl {
    template <typename Tag, typename U>
    static std::true_type test(type_safe::strong_typedef<Tag, U>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};

struct StrongTypedefT {
    template <typename T>
    constexpr bool operator()(boost::hana::basic_type<T> const&) const {
        return StrongTypeDefImpl<T>::value;
    }
};

constexpr StrongTypedefT strongTypeDef;
#endif

/// Tests if type is `type_safe::constrained_type`
/// \ingroup group-meta
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto constrainedType = [](auto type) { return isConstrainedType(type); };
#else
template <typename T>
struct ConstrainedTypeImpl {
    template <typename U, class Constraint, class Verifier>
    static std::true_type test(type_safe::constrained_type<U, Constraint, Verifier>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};

struct ConstrainedTypeT {
    template <typename T>
    constexpr bool operator()(boost::hana::basic_type<T> const&) const {
        return ConstrainedTypeImpl<T>::value;
    }
};

constexpr ConstrainedTypeT constrainedType;
#endif

/// Tests if type is `type_safe::integer`
/// \ingroup group-meta
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto integerType = [](auto type) { return isIntegerType(type); };
#else
template <typename T>
struct IntegerTypeImpl {
    template <typename IntegerT, class Policy /* = arithmetic_policy_default*/>
    static std::true_type test(type_safe::integer<IntegerT, Policy>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};

struct IntegerTypeT {
    template <typename T>
    constexpr bool operator()(boost::hana::basic_type<T> const&) const {
        return IntegerTypeImpl<T>::value;
    }
};

constexpr IntegerTypeT integerType;
#endif

/// Tests if type is `type_safe::floating_point`
/// \ingroup group-meta
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto floatingPoint = [](auto type) { return isFloatingPoint(type); };
#else
template <typename T>
struct FloatingPointTypeImpl {
    template <typename FloatT>
    static std::true_type test(type_safe::floating_point<FloatT>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};

struct FloatingPointTypeT {
    template <typename T>
    constexpr bool operator()(boost::hana::basic_type<T> const&) const {
        return FloatingPointTypeImpl<T>::value;
    }
};

constexpr FloatingPointTypeT floatingPoint;
#endif

/// Tests if type is `type_safe::boolean`
/// \ingroup group-meta
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto boolean = [](auto type) { return isBoolean(type); };
#else
template <typename T>
struct BooleanTypeImpl : std::false_type {};

template <>
struct BooleanTypeImpl<type_safe::boolean> : std::true_type {};

struct BooleanTypeT {
    template <typename T>
    constexpr bool operator()(boost::hana::basic_type<T> const&) const {
        return BooleanTypeImpl<T>::value;
    }
};

constexpr BooleanTypeT boolean;
#endif
#endif

/// Test if `T` has `T::strings()` static member
/// \ingroup group-meta
constexpr auto const hasStrings = boost::hana::is_valid(
        [](auto t) -> decltype((void)decltype(t)::type::strings()) {});

/// Compile time `to_string` for integer
#ifdef YENXO_DOXYGEN_INVOKED
template <int I>
constexpr auto cToString(boost::hana::int_<I>) noexcept {
    ...
}
#else
constexpr auto cToString(boost::hana::int_<0>) noexcept {
    return BOOST_HANA_STRING("");
}

template <int I>
constexpr auto cToString(boost::hana::int_<I>) noexcept {
    constexpr auto cl = I / 10;
    return cToString(boost::hana::int_c<cl>) + boost::hana::string_c<(I % 10) + 48>;
}
#endif

namespace detail {

template <typename T>
struct IsStdArrayImpl : std::false_type {};

template <typename T, size_t N>
struct IsStdArrayImpl<std::array<T, N>> : std::true_type {};

template <typename T>
struct StdArraySizeImpl;

template <typename T, size_t N>
struct StdArraySizeImpl<std::array<T, N>> {
    static constexpr auto value = N;
};

} // namespace detail

/// \ingroup group-meta
/// Is `type` a `std::variant`-like type.
inline constexpr auto isStdVariant = boost::hana::is_valid(
        [](auto type) -> decltype((void)yenxo::detail::Valid<std::variant_alternative_t<
                                          0,
                                          typename decltype(type)::type>>::value) {});

} // namespace yenxo
