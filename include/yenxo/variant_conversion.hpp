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
#include <yenxo/enum_traits.hpp>
#include <yenxo/exception.hpp>
#include <yenxo/meta.hpp>
#include <yenxo/variant.hpp>
#include <yenxo/when.hpp>

#if YENXO_ENABLE_TYPE_SAFE
#include <type_safe/strong_typedef.hpp>
#endif

#include <boost/hana.hpp>

#include <sstream>
#include <type_traits>
#include <variant>

namespace yenxo {

/// \ingroup group-details
/// Tests if type `T` has `static Variant T::toVariant(T)`.
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto hasToVariant = [](auto type) {
    return exists(static Variant type::toVariant(type));
};
#else
template <typename T>
struct HasToVariantImpl {
    static void var(Variant const&);
    template <typename U, typename = decltype(var(U::toVariant(std::declval<U>())))>
    static std::true_type test(boost::hana::basic_type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(boost::hana::type_c<T>))();
};

struct HasToVariantT {
    template <typename T>
    constexpr auto operator()(boost::hana::basic_type<T> const&) const {
        return HasToVariantImpl<T>::value;
    }
};

constexpr HasToVariantT hasToVariant;
#endif

/// \ingroup group-details
/// Tests if type `T` has `static T T::fromVariant(Variant)`.
#ifdef YENXO_DOXYGEN_INVOKED
inline constexpr auto hasFromVariant = [](auto type) {
    return exists(static type type::fromVariant(Variant));
};
#else
template <typename T>
struct HasFromVariantImpl {
    static void var(T const&);
    template <typename U,
              typename = decltype(var(U::fromVariant(std::declval<Variant>())))>
    static std::true_type test(boost::hana::basic_type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(boost::hana::type_c<T>))();
};

struct HasFromVariantT {
    template <typename T>
    constexpr auto operator()(boost::hana::basic_type<T> const&) const {
        return HasFromVariantImpl<T>::value;
    }
};

constexpr HasFromVariantT hasFromVariant;
#endif

/// \ingroup group-details
/// Is `type` a `Variant`.
inline constexpr auto isVariant = [](auto type) {
    using T = typename decltype(type)::type;
    return std::is_same_v<yenxo::Variant, T>;
};

/// \ingroup group-details
/// Is `type` convertible to any to build-in type supported by Variant.
inline constexpr auto isConvertibleToVariantBuildIn = [](auto type) {
    using T = typename decltype(type)::type;
    return !std::is_enum_v<T> && Variant::Types::convertible<T>();
};

/// \ingroup group-details
/// Is `type` any build-in type supported by Variant.
inline constexpr auto isVariantBuildIn = [](auto type) {
    using T = typename decltype(type)::type;
    return !std::is_enum_v<T> && Variant::Types::anyOf<T>();
};

/// \ingroup group-details
/// Is `type` a map-like type.
inline constexpr auto isMapType = [](auto type) {
    using T = typename decltype(type)::type;
    if constexpr (isContainer(type)) {
        return isKeyValue(boost::hana::type_c<typename T::value_type>)
            && !std::is_same_v<T, yenxo::VariantMap>;
    } else {
        return false;
    }
};

/// \ingroup group-details
/// Is `type` a collection-like type.
inline constexpr auto isCollectionType = [](auto type) {
    using T = typename decltype(type)::type;
    if constexpr (isContainer(boost::hana::type_c<T>)) {
        return !isKeyValue(boost::hana::type_c<typename T::value_type>)
            && !std::is_same_v<T, yenxo::VariantVec>;
    } else {
        return false;
    }
};

/// \ingroup group-details
/// Is `type` an enum type with specialized `EnumTraits`.
inline constexpr auto isReflectiveEnum = boost::hana::is_valid(
        [](auto type) -> decltype(
                              (void)EnumTraits<typename decltype(type)::type>::toString(
                                      std::declval<typename decltype(type)::type>())) {});

/// \ingroup group-details
/// Check if `type` is an enum type with specialized `EnumTraits`, and the specialization
/// does not define `static auto strings()`.
inline constexpr auto isReflectiveEnumWithSingleStringRepresentation = [](auto type) {
    return isReflectiveEnum(type)
        && !hasStrings(boost::hana::type_c<EnumTraits<typename decltype(type)::type>>);
};

/// \ingroup group-details
/// Check if `type` is an enum type with specialized `EnumTraits`, the specialization does
/// define `static auto strings()`
inline constexpr auto isReflectiveEnumWithMultiStringRepresentation = [](auto type) {
    return isReflectiveEnum(type)
        && hasStrings(boost::hana::type_c<EnumTraits<typename decltype(type)::type>>);
};

/// \ingroup group-details
/// Is `type` a collection-like type with `push_back` method.
inline constexpr auto isCollectionTypeWithPushBack = [](auto type) {
    using T = typename decltype(type)::type;
    if constexpr (isContainer(type)) {
        return !isKeyValue(boost::hana::type_c<typename T::value_type>)
            && hasPushBack(type) && !std::is_same_v<T, yenxo::VariantVec>;
    } else {
        return false;
    }
};

/// \ingroup group-details
/// Is `type` a collection-like type with `emplace` method.
inline constexpr auto isCollectionTypeWithEmplace = [](auto type) {
    using T = typename decltype(type)::type;
    if constexpr (isContainer(type)) {
        return !isKeyValue(boost::hana::type_c<typename T::value_type>)
            && !hasPushBack(type) && hasEmplace(type)
            && !std::is_same_v<T, yenxo::VariantVec>;
    } else {
        return false;
    }
};

/// To `Variant` conversion function object
/// \ingroup group-function
///
/// The function object is enabled for any type `T` for which
/// `toVariantConvertible(boost::hana::type_c<T>)` returns `true`.
#ifdef YENXO_DOXYGEN_INVOKED
auto toVariant = [](auto value) { return Variant(yenxo(value)); };
#else
struct ToVariantT {
    template <typename T>
    auto operator()(T&& args) const;
};

constexpr ToVariantT toVariant;

// Unified conversion of T to Variant
template <typename T, typename = void>
struct ToVariantImpl : ToVariantImpl<T, When<true>> {};

// Fallback
template <typename T, bool condition>
struct ToVariantImpl<T, When<condition>> {
    [[noreturn]] static Variant apply(T const&) {
        static_assert(T::is_not_convertible_to_variant);
        throw 0;
    }
};

template <typename T>
struct ToVariantImpl<T, When<isVariant(boost::hana::type_c<T>)>> {
    static Variant apply(T const& x) {
        return x;
    }
};

// Specialization for types with `static Variant T::toVariant(T)`
template <typename T>
struct ToVariantImpl<T, When<hasToVariant(boost::hana::type_c<T>)>> {
    static Variant apply(T const& x) {
        return T::toVariant(x);
    }
};

// Specialization for `Variant` built-in supported types
template <typename T>
struct ToVariantImpl<T, When<isConvertibleToVariantBuildIn(boost::hana::type_c<T>)>> {
    static Variant apply(T x) {
        return Variant(x);
    }
};

// Specialization for map types
template <typename T>
struct ToVariantImpl<T, When<isMapType(boost::hana::type_c<T>)>> {
    static Variant apply(T const& map) {
        VariantMap ret;
        for (auto const& x : map) {
            ret.emplace(ToVariantImpl<typename T::key_type>::apply(x.first),
                        ToVariantImpl<typename T::mapped_type>::apply(x.second));
        }
        return Variant(ret);
    }
};

// Specialization for pair
template <typename T>
struct ToVariantImpl<T, When<isPair(boost::hana::type_c<T>)>> {
    static Variant apply(T const& pair) {
        VariantMap tmp;
        tmp["first"] = toVariant(pair.first);
        tmp["second"] = toVariant(pair.second);
        return Variant(tmp);
    }
};

// Specialization for collection types
template <typename T>
struct ToVariantImpl<T, When<isCollectionType(boost::hana::type_c<T>)>> {
    static Variant apply(T const& vec) {
        VariantVec ret;
        ret.reserve(std::size(vec));
        for (auto const& x : vec) {
            ret.push_back(toVariant(x));
        }
        return Variant(ret);
    }
};

// Specialization for types with specialized EnumTraits
template <class T>
struct ToVariantImpl<T, When<isReflectiveEnum(boost::hana::type_c<T>)>> {
    static Variant apply(T e) {
        return Variant(EnumTraits<T>::toString(e));
    }
};

#if YENXO_ENABLE_TYPE_SAFE
// Specialization for `type_safe::strong_typedef`
template <typename T>
struct ToVariantImpl<T,
                     When<!hasToVariant(boost::hana::type_c<T>)
                          && strongTypeDef(boost::hana::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = type_safe::underlying_type<T>;
        return ToVariantImpl<U>::apply(static_cast<U const&>(st));
    }
};

// Specialization for `type_safe::constrained_type`
template <typename T>
struct ToVariantImpl<T, When<constrainedType(boost::hana::type_c<T>)>> {
    static Variant apply(T const& st) {
        return ToVariantImpl<typename T::value_type>::apply(st.get_value());
    }
};

// Specialization for `type_safe::integer`
template <typename T>
struct ToVariantImpl<T, When<integerType(boost::hana::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::integer_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};

// Specialization for `type_safe::floating_point`
template <typename T>
struct ToVariantImpl<T, When<floatingPoint(boost::hana::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::floating_point_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};

// Specialization for `type_safe::boolean`
template <typename T>
struct ToVariantImpl<T, When<boolean(boost::hana::type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = bool;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};
#endif

// `hana::map`
template <typename T>
struct ToVariantImpl<T, When<boost::hana::is_a<boost::hana::map_tag, T>>> {
    static Variant apply(T const& map) {
        Variant::Map ret;
        boost::hana::for_each(map, boost::hana::fuse([&ret](auto key, auto value) {
                                  ret[boost::hana::to<char const*>(key)] =
                                          toVariant(value);
                              }));
        return Variant(ret);
    }
};

// `hana::string`
template <typename T>
struct ToVariantImpl<T, When<boost::hana::is_a<boost::hana::string_tag, T>>> {
    static Variant apply(T const& var) {
        return Variant(boost::hana::to<char const*>(var));
    }
};

// `hana::tuple`
template <class T>
struct ToVariantImpl<T, When<boost::hana::is_a<boost::hana::tuple_tag, T>>> {
    static Variant apply(T const& val) {
        VariantVec ret;
        ret.reserve(boost::hana::size(val));
        boost::hana::for_each(val,
                              [&ret](auto const& x) { ret.push_back(toVariant(x)); });
        return Variant(ret);
    }
};

// `std::variant`
template <typename T>
struct ToVariantImpl<T, When<isStdVariant(boost::hana::type_c<T>)>> {
    static Variant apply(T const& var) {
        return std::visit(Overload{[](auto const& x) { return toVariant(x); }}, var);
    }
};

template <typename T>
auto ToVariantT::operator()(T&& val) const {
    return ToVariantImpl<std::decay_t<T>>::apply(std::forward<T>(val));
}
#endif

/// From `Variant` conversion function object
/// \ingroup group-function
///
/// The function object is enabled for any type `T` for which
/// `toVariantConvertible(boost::hana::type_c<T>)` returns `true`.
#ifdef YENXO_DOXYGEN_INVOKED
template <class T>
constexpr auto fromVariant = [](Variant const& var) { return T(deserialize(var)); };
#else
template <typename T>
struct FromVariantT {
    auto operator()(Variant const& x) const;
};

// Convenient shortcut function
template <typename T>
constexpr FromVariantT<T> fromVariant;

// Unified conversion of Variant to T
template <typename T, typename = void>
struct FromVariantImpl : FromVariantImpl<T, When<true>> {};

// Fallback
template <typename T, bool condition>
struct FromVariantImpl<T, When<condition>> {
    [[noreturn]] static T apply(Variant const&) {
        static_assert(T::is_not_convertible_from_variant);
        throw 0;
    }
};

// Identity
template <typename T>
struct FromVariantImpl<T, When<std::is_same_v<yenxo::Variant, T>>> {
    static Variant apply(T const& x) {
        return x;
    }
};

// Specialization for types with `static T T::fromVariant(Variant)`
template <typename T>
struct FromVariantImpl<T, When<hasFromVariant(boost::hana::type_c<T>)>> {
    static T apply(Variant const& x) {
        return T::fromVariant(x);
    }
};

// Specialization for `Variant` built-in supported types
template <typename T>
struct FromVariantImpl<T, When<isVariantBuildIn(boost::hana::type_c<T>)>> {
    static T apply(Variant const& x) {
        return static_cast<T>(x);
    }
};

namespace detail {

template <class F>
inline void tryCatch(F&& f, size_t i) {
    try {
        f();
    } catch (yenxo::VariantErr& e) {
        e.prependPath(std::to_string(i));
        throw;
    } catch (std::exception const& e) {
        VariantErr err(e.what());
        err.prependPath(std::to_string(i));
        throw std::move(err);
    }
}

template <class F, class S>
inline void tryCatch(F&& f, S s) {
    try {
        f();
    } catch (yenxo::VariantErr& e) {
        e.prependPath(boost::hana::to<char const*>(s));
        throw;
    } catch (std::exception const& e) {
        VariantErr err(e.what());
        err.prependPath(boost::hana::to<char const*>(s));
        throw std::move(err);
    }
}

} // namespace detail

template <typename T>
struct FromVariantImpl<T, When<detail::IsStdArrayImpl<T>::value>> {
    static T apply(Variant const& var) {
        auto const& vec = var.vec();
        constexpr auto N = detail::StdArraySizeImpl<T>::value;
        if (vec.size() != N) {
            throw std::logic_error("expected size of the list is " + std::to_string(N)
                                   + ", actual " + std::to_string(vec.size()));
        }
        T ret;
        for (size_t i = 0; i < N; ++i) {
            detail::tryCatch(
                    [&] { ret[i] = fromVariant<typename T::value_type>(vec[i]); }, i);
        }
        return ret;
    }
};

// Specialization for collection types (with push_back)
template <typename T>
struct FromVariantImpl<T, When<isCollectionTypeWithPushBack(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        T ret;
        size_t i = 0;
        for (auto const& x : var.vec()) {
            detail::tryCatch(
                    [&] { ret.push_back(fromVariant<typename T::value_type>(x)); }, i++);
        }
        return ret;
    }
};

// Specialization for collection types (with emplace)
template <typename T>
struct FromVariantImpl<T, When<isCollectionTypeWithEmplace(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        T ret;
        size_t i = 0;
        for (auto const& x : var.vec()) {
            detail::tryCatch([&] { ret.emplace(fromVariant<typename T::value_type>(x)); },
                             i++);
        }
        return ret;
    }
};

// Specialization for map types
template <typename T>
struct FromVariantImpl<T, When<isMapType(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        T ret;
        for (auto const& x : var.map()) {
            ret.emplace(FromVariantImpl<typename T::key_type>::apply(Variant(x.first)),
                        FromVariantImpl<typename T::mapped_type>::apply(x.second));
        }
        return ret;
    }
};

// Specialization for pair
template <typename T>
struct FromVariantImpl<T, When<isPair(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(yenxo::fromVariant<typename T::first_type>(var.map().at("first")),
                 yenxo::fromVariant<typename T::second_type>(var.map().at("second")));
    }
};

// Specialization for types with specialized EnumTraits
template <class T>
struct FromVariantImpl<
        T,
        When<isReflectiveEnumWithSingleStringRepresentation(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        auto const& s = var.str();
        for (auto e : EnumTraits<T>::values) {
            if (EnumTraits<T>::toString(e) == s) {
                return e;
            }
        }
        throw VariantBadType(s, boost::hana::type_c<T>);
    }
};

// Specialization for types with specialized EnumTraits
template <class T>
struct FromVariantImpl<
        T,
        When<isReflectiveEnumWithMultiStringRepresentation(boost::hana::type_c<T>)>> {
    template <size_t I>
    static typename EnumTraits<T>::Enum applyImpl(boost::hana::size_t<I>,
                                                  std::string const& x) {
        bool found{false};
        boost::hana::for_each(boost::hana::at_c<I>(EnumTraits<T>::strings()),
                              [&](auto s) { found |= strcmp(s, x.c_str()) == 0; });
        if (found) {
            return EnumTraits<T>::values[I];
        } else {
            return applyImpl(boost::hana::size_c<I + 1>, x);
        }

        return EnumTraits<T>::values[I];
    }

    static typename EnumTraits<T>::Enum applyImpl(
            boost::hana::size_t<EnumTraits<T>::count>,
            std::string const& x) {
        throw VariantBadType(x, boost::hana::type_c<T>);
    }

    static T apply(std::string const& x) {
        return applyImpl(boost::hana::size_c<0>, x);
    }

    static T apply(Variant const& var) {
        auto const& s = var.str();
        return applyImpl(boost::hana::size_c<0>, s);
    }
};

#if YENXO_ENABLE_TYPE_SAFE
// Specialization for `type_safe::strong_typedef`
template <typename T>
struct FromVariantImpl<T,
                       When<!hasFromVariant(boost::hana::type_c<T>)
                            && strongTypeDef(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        using U = type_safe::underlying_type<T>;
        return static_cast<T>(FromVariantImpl<U>::apply(var));
    }
};

// Specialization for `type_safe::constrained_type`
template <typename T>
struct FromVariantImpl<T, When<constrainedType(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::value_type>::apply(var));
    }
};

// Specialization for `type_safe::integer`
template <typename T>
struct FromVariantImpl<T, When<integerType(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::integer_type>::apply(var));
    }
};

// Specialization for `type_safe::floating_point`
template <typename T>
struct FromVariantImpl<T, When<floatingPoint(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::floating_point_type>::apply(var));
    }
};

// Specialization for `type_safe::boolean`
template <typename T>
struct FromVariantImpl<T, When<boolean(boost::hana::type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<bool>::apply(var));
    }
};
#endif

// `hana::map`
template <typename T>
struct FromVariantImpl<T, When<boost::hana::is_a<boost::hana::map_tag, T>>> {
    static T apply(Variant const& var) {
        auto const map = var.map();
        T ret;
        boost::hana::for_each(
                ret, boost::hana::fuse([&](auto key, auto& value) {
                    using namespace std::string_literals;
                    auto const it = map.find(boost::hana::to<char const*>(key));
                    if (map.end() == it) {
                        throw std::logic_error(boost::hana::to<char const*>(key)
                                               + " is required"s);
                    }
                    detail::tryCatch(
                            [&] { value = fromVariant<decltype(value)>(it->second); },
                            key);
                }));
        return ret;
    }
};

// `hana::string`
template <typename T>
struct FromVariantImpl<T, When<boost::hana::is_a<boost::hana::string_tag, T>>> {
    static T apply(Variant const& var) {
        if (var.str() != boost::hana::to<char const*>(T())) {
            std::ostringstream oss;
            oss << var;
            throw VariantBadType(oss.str(), boost::hana::type_c<T>);
        }
        return T();
    }
};

// `hana::tuple`
template <typename T>
struct FromVariantImpl<T, When<boost::hana::is_a<boost::hana::tuple_tag, T>>> {
    static T apply(Variant const& var) {
        T ret;
        constexpr const auto N = boost::hana::size(ret);
        auto const& vec = var.vec();
        if (vec.size() != boost::hana::size(ret)) {
            throw std::logic_error("expected size of the tuple is " + std::to_string(N)
                                   + ", actual " + std::to_string(vec.size()));
        }
        boost::hana::for_each(
                boost::hana::make_range(boost::hana::size_c<0>, boost::hana::size_c<N>),
                [&](auto i) {
                    detail::tryCatch(
                            [&] { ret[i] = fromVariant<decltype(ret[i])>(vec[i]); },
                            boost::hana::value(i));
                });
        return ret;
    }
};

// `hana::Constant`
template <typename T>
struct FromVariantImpl<T, When<boost::hana::Constant<T>().value>> {
    static T apply(Variant const& var) {
        auto const tmp = fromVariant<typename T::value_type>(var);
        if (tmp != T::value) {
            std::ostringstream oss;
            oss << var;
            throw VariantBadType(oss.str(), boost::hana::type_c<T>);
        }
        return T();
    }
};

// `std::variant`
template <typename T>
struct FromVariantImpl<
        T,
        When<yenxo::detail::Valid<std::variant_alternative_t<0, T>>::value>> {
    template <size_t I>
    static T applyImpl(boost::hana::size_t<I>, Variant const& var) {
        try {
            return fromVariant<std::variant_alternative_t<I, T>>(var);
        } catch (...) {
            return applyImpl(boost::hana::size_c<I + 1>, var);
        }
    }

    [[noreturn]] static T applyImpl(boost::hana::size_t<std::variant_size_v<T>>,
                                    Variant const& var) {
        std::ostringstream os;
        os << var;
        throw VariantBadType(os.str(), boost::hana::type_c<T>);
    }

    static T apply(Variant const& var) {
        return applyImpl(boost::hana::size_c<0>, var);
    }
};

template <typename T>
auto FromVariantT<T>::operator()(Variant const& x) const {
    return FromVariantImpl<std::remove_cv_t<std::remove_reference_t<T>>>::apply(x);
}
#endif

/// To `Variant` conversion function object
/// \ingroup group-function
/// \see toVariant
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto toVariant2 = [](Variant& out, auto value) { out = toVariant(value); };
#else
struct ToVariantT2 {
    template <class T>
    void operator()(Variant& var, T&& val) const {
        var = toVariant(std::forward<T>(val));
    }
};

constexpr ToVariantT2 toVariant2;
#endif

/// From `Variant` conversion function object
/// \ingroup group-function
/// \see fromVariant
#ifdef YENXO_DOXYGEN_INVOKED
constexpr auto fromVariant2 = [](auto& value, Variant const& var) {
    value = fromVariant<decltype(type)>(var);
};
#else
struct FromVariantT2 {
    template <class T>
    void operator()(T& val, Variant const& var) const {
        val = fromVariant<T>(var);
    }
};

constexpr FromVariantT2 fromVariant2;
#endif

/// Whether `T` can be converted to Variant
/// \ingroup group-traits
///
/// The intrusive way to enable the trait for user defined type `T` is to
/// implement `Variant toVariant(T const&)` static member for `T`.
///
/// The non-intrusive way to enable the trait for user defined type `T` is to
/// provide specialization
/// \code
/// namespace yenxo {
/// template <>
/// struct ToVariantImpl<T> {
///     Variant apply(T const&) {
/// 		// implementation
/// 	}
/// };
/// }
/// \endcode
inline constexpr auto toVariantConvertible = [](auto type) {
    return isVariant(type) || hasToVariant(type) || isConvertibleToVariantBuildIn(type)
        || isMapType(type) || isCollectionType(type) || isReflectiveEnum(type)
#if YENXO_ENABLE_TYPE_SAFE
        || (!hasToVariant(type) && strongTypeDef(type)) || constrainedType(type)
        || integerType(type) || floatingPoint(type) || boolean(type)
#endif
        || boost::hana::is_a<
                   boost::hana::map_tag,
                   typename decltype(type)::
                           type> || boost::hana::is_a<boost::hana::string_tag, typename decltype(type)::type> || boost::hana::is_a<boost::hana::tuple_tag, typename decltype(type)::type> || isStdVariant(type);
};

/// Whether `T` can be constructed from Variant
/// \ingroup group-traits
///
/// The intrusive way to enable the trait for user defined type `T` is to
/// implement `T fromVariant(Variant const&)` static member for `T`
///
/// The non-intrusive way to enable the trait for user defined type `T` is to
/// provide specialization
/// \code
/// namespace yenxo {
/// template <>
/// struct FromVariantImpl<T> {
///     T apply(Variant const&) {
/// 		// implementation
/// 	}
/// };
/// }
/// \endcode
inline constexpr auto fromVariantConvertible = [](auto type) {
    return    isVariant(type)
           || hasFromVariant(type)
           || isVariantBuildIn(type)
           || boost::hana::template_<detail::IsStdArrayImpl>(type)
           || isCollectionTypeWithPushBack(type)
           || isCollectionTypeWithEmplace(type)
           || isMapType(type)
           || isPair(type)
           || isReflectiveEnumWithSingleStringRepresentation(type)
           || isReflectiveEnumWithMultiStringRepresentation(type)
#if YENXO_ENABLE_TYPE_SAFE
           || (!hasToVariant(type) && strongTypeDef(type))
           || constrainedType(type)
           || integerType(type)
           || floatingPoint(type)
           || boolean(type)
#endif
           || boost::hana::is_a<boost::hana::map_tag, typename decltype(type)::type>
           || boost::hana::is_a<boost::hana::string_tag, typename decltype(type)::type>
           || boost::hana::is_a<boost::hana::tuple_tag, typename decltype(type)::type>
           || boost::hana::Constant<typename decltype(type)::type>().value
           || isStdVariant(type);
};

} // namespace yenxo
