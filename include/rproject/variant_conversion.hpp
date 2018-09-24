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
#include <meta.hpp>
#include <variant.hpp>
#include <when.hpp>

// 3rd
#include <type_safe/strong_typedef.hpp>

// std
#include <type_traits>


namespace mixin::detail {


///
/// Unified converstion of T to Variant
///
template <typename T, typename = void>
struct ToVariantImpl : ToVariantImpl<T, When<true>> {};


///
/// Specialization to types with `static Variant T::toVariant(T)`
///
template <typename T, bool condition>
struct ToVariantImpl<T, When<condition>> {
    static_assert(
            rp::callable(T::toVariant, rp::type_c<T>),
            "The type T must have T::toVariant(T) static member function");
    static_assert(
            std::is_same_v<decltype(T::toVariant(std::declval<T>())), Variant>,
            "The return type of T::toVariant must be Variant");

    static Variant apply(T const& x) { return T::toVariant(x); }
};


///
/// Specialization for types for which Variant have constructor
///
template <typename T>
struct ToVariantImpl<T, When<rp::detail::Valid<
        decltype(Variant(std::declval<T>()))>::value>> {
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
        Variant::Map ret;
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
        Variant::Vec ret;
        for (auto const& x: vec) {
            ret.push_back(ToVariantImpl<std::decay_t<decltype(x)>>::apply(x));
        }
        return Variant(ret);
    }
};


///
/// Specialization for `type_safe::strong_typedef`s
///
template <typename T>
struct ToVariantImpl<T, When<rp::strongTypeDef(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = type_safe::underlying_type<T>;
        return ToVariantImpl<U>::apply(static_cast<U const&>(st));
    }
};


///
/// Specialization for `type_safe::constrained_type`s
///
template <typename T>
struct ToVariantImpl<T, When<rp::constrainedType(rp::type_c<T>)>> {
    static Variant apply(T const& st) {
        return ToVariantImpl<std::decay_t<typename T::value_type>>::apply(
                    st.get_value());
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
constexpr ToVariantT toVariant;


///
/// Unified converstion of Variant to T
///
template <typename T, typename = void>
struct FromVariantImpl : FromVariantImpl<T, When<true>> {};


///
/// Specialization to types with `static T T::fromVariant(Variant)`
///
template <typename T, bool condition>
struct FromVariantImpl<T, When<condition>> {
    static_assert(
            rp::callable(T::fromVariant, rp::type_c<Variant>),
            "The type T must have static member function fromVariant(Variant)");
    static_assert(
            std::is_same_v<decltype(T::fromVariant(Variant())), T>,
            "The return type of T::fromVariant must be T");

    static T apply(Variant const& x) { return T::fromVariant(x); }
};


///
/// Specialization for types for which Variant have conversion
///
template <typename T>
struct FromVariantImpl<T,
        When<rp::detail::Valid<decltype(static_cast<T>(Variant()))>::value>> {
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
struct FromVariantImpl<T, When<rp::strongTypeDef(rp::type_c<T>)>> {
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


struct FromVariantT {
    template <typename T>
    auto operator()(Variant const& x) const {
        return FromVariantImpl<std::decay_t<T>>::apply(x);
    }

    template <typename T>
    void operator()(T& x, Variant const& y) const {
        x = FromVariantImpl<std::decay_t<T>>::apply(y);
    }
};


///
/// Convinient shortcut function
///
constexpr FromVariantT fromVariant;


} // namespace mixin::detail
