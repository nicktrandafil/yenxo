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


// local
#include <rproject/meta.hpp>
#include <rproject/variant.hpp>
#include <rproject/when.hpp>

// 3rd
#include <type_safe/strong_typedef.hpp>

// boost
#include <boost/hana.hpp>

// std
#include <type_traits>


namespace trait {


template <typename T>
struct HasToVariantImpl {
    static void var(Variant const&);
    template <typename U, typename = decltype(
              var(U::toVariant(std::declval<U>())))>
    static std::true_type test(rp::Type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(rp::type_c<T>))();
};


struct HasToVariantT {
    template <typename T>
    constexpr auto operator()(rp::Type<T> const&) const {
        return HasToVariantImpl<std::decay_t<T>>::value;
    }
};


///
/// Tests if type has Variant T::toVariant(T)
///
constexpr HasToVariantT hasToVariant;


template <typename T>
struct HasFromVariantImpl {
    static void var(T const&);
    template <typename U, typename = decltype(
              var(U::fromVariant(std::declval<Variant>())))>
    static std::true_type test(rp::Type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(rp::type_c<T>))();
};


struct HasFromVariantT {
    template <typename T>
    constexpr auto operator()(rp::Type<T> const&) const {
        return HasFromVariantImpl<std::decay_t<T>>::value;
    }
};


///
/// Tests if type has T T::fromVariant(Variant)
///
constexpr HasFromVariantT hasFromVariant;


///
/// Unified converstion of T to Variant
///
template <typename T, typename = void>
struct ToVariantImpl : ToVariantImpl<T, When<true>> {};


///
/// Fallback
///
template <typename T, bool condition>
struct ToVariantImpl<T, When<condition>> {
    static Variant apply(T const&) {
        static_assert(rp::DependentFalse<T>::value,
                      "No conversion to Variant is provided");
        return {};
    }
};


///
/// Specialization to types with `static Variant T::toVariant(T)`
///
template <typename T>
struct ToVariantImpl<T, When<hasToVariant(rp::type_c<T>)>> {
    static Variant apply(T const& x) { return T::toVariant(x); }
};


///
/// Specialization Variant build-in supported types
///
template <typename T>
struct ToVariantImpl<T, When<Variant::Types::convertible<T>()>> {
    static Variant apply(T x) { return Variant(x); }
};


///
/// Specialization for `Container`
///
template <typename T>
struct ToVariantImpl<T,
        When<rp::isContainer(rp::type_c<T>) &&
             rp::isKeyValue(rp::type_c<typename T::value_type>)>> {
    static Variant apply(T const& map) {
        VariantMap ret;
        for (auto const& x: map) {
            ret.emplace(
                ToVariantImpl<typename T::key_type>::apply(x.first),
                ToVariantImpl<typename T::mapped_type>::apply(x.second));
        }
        return Variant(ret);
    }
};


///
/// Specialization for `Dict`
///
template <typename T>
struct ToVariantImpl<T,
        When<rp::isContainer(rp::type_c<T>) &&
             !rp::isKeyValue(rp::type_c<typename T::value_type>)>> {
    static Variant apply(T const& vec) {
        VariantVec ret;
        for (auto const& x: vec) {
            ret.push_back(ToVariantImpl<std::decay_t<decltype(x)>>::apply(x));
        }
        return Variant(ret);
    }
};


///
/// Specialization for `type_safe::strong_typedef`
///
template <typename T>
struct ToVariantImpl<T, When<
        !hasToVariant(rp::type_c<T>) &&
        rp::strongTypeDef(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = type_safe::underlying_type<T>;
        return ToVariantImpl<U>::apply(static_cast<U const&>(st));
    }
};


///
/// Specialization for `type_safe::constrained_type`
///
template <typename T>
struct ToVariantImpl<T, When<rp::constrainedType(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        return ToVariantImpl<std::decay_t<typename T::value_type>>::apply(
                    st.get_value());
    }
};


///
/// Specialization for `type_safe::integer`
///
template <typename T>
struct ToVariantImpl<T, When<rp::integerType(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::integer_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};


///
/// Specialization for `type_safe::floating_point`
///
template <typename T>
struct ToVariantImpl<T, When<rp::floatingPoint(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::floating_point_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};


///
/// Specialization for `type_safe::boolean`
///
template <typename T>
struct ToVariantImpl<T, When<rp::boolean(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = bool;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};


struct ToVariantT {
    template <typename ...Args>
    auto operator()(Args&&... args) const {
        return ToVariantImpl<std::decay_t<Args>...>::apply(
                    std::forward<Args>(args)...);
    }
};


///
/// Convinient shortcut function
///
//constexpr ToVariantT toVariant;


template <typename T>
constexpr auto toVariant(T&& x) {
    return ToVariantImpl<std::decay_t<T>>::apply(std::forward<T>(x));
}


///
/// Unified converstion of Variant to T
///
template <typename T, typename = void>
struct FromVariantImpl : FromVariantImpl<T, When<true>> {};


///
/// Fallback
///
template <typename T, bool condition>
struct FromVariantImpl<T, When<condition>> {
    static T apply(Variant const&) {
        static_assert(rp::DependentFalse<T>::value,
                      "No conversion from Variant is provided");
        return {};
    }
};


///
/// Specialization to types with `static T T::fromVariant(Variant)`
///
template <typename T>
struct FromVariantImpl<T, When<hasFromVariant(rp::type_c<T>)>> {
    static T apply(Variant const& x) { return T::fromVariant(x); }
};


///
/// Specialization for types for which Variant have conversion
///
template <typename T>
struct FromVariantImpl<T, When<Variant::Types::convertible<T>()>> {
    static T apply(Variant const& x) { return static_cast<T>(x); }
};


///
/// Specialization for `Container`
///
template <typename T>
struct FromVariantImpl<T, When<rp::isContainer(rp::type_c<T>) &&
        !rp::isKeyValue(rp::type_c<typename T::value_type>)>> {
    static T apply(Variant const& var) {
        T ret;
        for (auto const& x: var.vec()) {
            ret.push_back(FromVariantImpl<typename T::value_type>::apply(x));
        }
        return ret;
    }
};


///
/// Specialization for `Dict`
///
template <typename T>
struct FromVariantImpl<T, When<rp::isContainer(rp::type_c<T>) &&
        rp::isKeyValue(rp::type_c<typename T::value_type>)>> {
    static T apply(Variant const& var) {
        T ret;
        for (auto const& x: var.map()) {
            ret.emplace(
                FromVariantImpl<typename T::key_type>::apply(Variant(x.first)),
                FromVariantImpl<typename T::mapped_type>::apply(x.second));
        }
        return ret;
    }
};


///
/// Specialization for `type_safe::strong_typedef`
///
template <typename T>
struct FromVariantImpl<T, When<
        !hasFromVariant(rp::type_c<T>) &&
        rp::strongTypeDef(rp::type_c<T>)>> {
    static T apply(Variant const& var) {
        using U = type_safe::underlying_type<T>;
        return static_cast<T>(FromVariantImpl<U>::apply(var));
    }
};


///
/// Specialization for `type_safe::constrained_type`
///
template <typename T>
struct FromVariantImpl<T, When<rp::constrainedType(rp::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<std::decay_t<typename T::value_type>>::apply(var));
    }
};


///
/// Specialization for `type_safe::integer`
///
template <typename T>
struct FromVariantImpl<T, When<rp::integerType(rp::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::integer_type>::apply(var));
    }
};


///
/// Specialization for `type_safe::floating_point`
///
template <typename T>
struct FromVariantImpl<T, When<rp::floatingPoint(rp::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::floating_point_type>::apply(var));
    }
};


///
/// Specialization for `type_safe::boolean`
///
template <typename T>
struct FromVariantImpl<T, When<rp::boolean(rp::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<bool>::apply(var));
    }
};


template <typename T>
struct FromVariantT {
    auto operator()(Variant const& x) const {
        return FromVariantImpl<std::decay_t<T>>::apply(x);
    }
};


///
/// Convinient shortcut function
///
template <typename T>
constexpr FromVariantT<T> fromVariant;


} // namespace trait
