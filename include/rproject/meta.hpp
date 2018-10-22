#pragma once


// local
#include <rproject/when.hpp>

// 3rd
#include <type_safe/strong_typedef.hpp>
#include <type_safe/constrained_type.hpp>
#include <type_safe/types.hpp>

// hana
#include <boost/hana/tuple.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/any_of.hpp>

// std
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>


namespace rp {


template <typename... Args>
struct S {
    template <typename T>
    using push_front = S<T, Args...>;

    template <template <class...> class T>
    using rebind = T<Args...>;

    template <typename Key>
    constexpr static bool convertible() {
        return boost::hana::any_of(
                    boost::hana::tuple<boost::hana::type<Args>...>{},
                    [](auto x) {
            return std::is_convertible_v<Key, typename decltype(x)::type>;
        });
    }

    template <typename Key>
    constexpr static bool anyOf() {
        return boost::hana::type_c<Key> ^boost::hana::in^
                boost::hana::tuple<boost::hana::type<Args>...>{};
    }
};


namespace detail {


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
constexpr Type<T> type_c;


template <typename F, typename ...Args>
constexpr bool callable(F&&, Type<Args> const&...) noexcept {
    return detail::Callable<F, S<Args...>>::value;
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


namespace detail {


template <typename T, typename = void>
struct IsIterable : IsIterable<T, When<true>> {};


template <typename T, bool condition>
struct IsIterable<T, When<condition>> : std::false_type {};


template <typename T>
struct IsIterable<
    T,
    When<valid(type_c<decltype((
                   // begin/end and operator !=
                   begin(std::declval<T&>()) != end(std::declval<T&>()),

                   // operator ++
                   ++std::declval<decltype(begin(std::declval<T&>()))&>(),

                   // operator*
                   *begin(std::declval<T&>())))
               >)>> : std::true_type

{};


template <typename T, typename = void>
struct IsString : IsString<T, When<true>> {};


template <typename T, bool condition>
struct IsString<T, When<condition>> : std::false_type {};


template <typename T>
struct IsString<
        T,
        When<std::is_convertible_v<T,
             std::basic_string<typename T::value_type,
                               typename T::traits_type,
                               typename T::allocator_type>>>> : std::true_type {};



template <typename T>
struct IsPair : std::false_type {};


template <typename First, typename Second>
struct IsPair<std::pair<First, Second>> : std::true_type {};


} // namespace detail


template <typename T>
constexpr auto isPair(Type<T> const&) {
    return detail::IsPair<T>::value;
}


template <typename T>
constexpr auto isString(Type<T> const&) {
    return detail::IsString<T>::value;
}


template <typename T>
constexpr auto isIterable(Type<T> const&) {
    return detail::IsIterable<T>::value;
}


template <typename T>
constexpr auto isContainer(Type<T> const& x) {
    return isIterable(x) && !isString(x);
}


template <typename T>
constexpr auto isKeyValue(Type<T> const& x) {
    if constexpr (isPair(x)) {
        return isString(type_c<typename T::first_type>);
    } else {
        return false;
    }
}


template <typename T>
struct StrongTypeDefImpl {
    template <typename Tag, typename U>
    static std::true_type test(type_safe::strong_typedef<Tag, U>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};


struct StrongTypedefT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return StrongTypeDefImpl<T>::value;
    }
};


///
/// Tests if type is `type_safe::strong_typedef`
///
constexpr StrongTypedefT strongTypeDef;


template <typename T>
struct ConstrainedTypeImpl {
    template <typename U, class Constraint, class Verifier>
    static std::true_type test(type_safe::constrained_type<U, Constraint, Verifier>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};


struct ConstrainedTypeT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return ConstrainedTypeImpl<T>::value;
    }
};


///
/// Tests if type is `type_safe::constrained_type`
///
constexpr ConstrainedTypeT constrainedType;


template <typename T>
struct IntegerTypeImpl {
    template <typename IntegerT, class Policy /* = arithmetic_policy_default*/>
    static std::true_type test(type_safe::integer<IntegerT, Policy>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};


struct IntegerTypeT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return IntegerTypeImpl<T>::value;
    }
};


///
/// Tests if type is `type_safe::integer`
///
constexpr IntegerTypeT integerType;


template <typename T>
struct FloatingPointTypeImpl {
    template <typename FloatT>
    static std::true_type test(type_safe::floating_point<FloatT>&&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(std::declval<T>()))();
};


struct FloatingPointTypeT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return FloatingPointTypeImpl<T>::value;
    }
};


///
/// Tests if type is `type_safe::integer`
///
constexpr FloatingPointTypeT floatingPoint;


template <typename T>
struct BooleanTypeImpl : std::false_type {};


template <>
struct BooleanTypeImpl<type_safe::boolean> : std::true_type {};


struct BooleanTypeT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return BooleanTypeImpl<T>::value;
    }
};


///
/// Tests if type is `type_safe::integer`
///
constexpr BooleanTypeT boolean;


} // namespace rp
