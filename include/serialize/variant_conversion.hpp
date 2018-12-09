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
#include <serialize/config.hpp>
#include <serialize/meta.hpp>
#include <serialize/variant.hpp>
#include <serialize/when.hpp>

// 3rd
#if SERIALIZE_ENABLE_TYPE_SAFE
#include <type_safe/strong_typedef.hpp>
#endif

// boost
#include <boost/hana.hpp>

// std
#include <type_traits>


namespace serialize {


template <typename T>
struct HasToVariantImpl {
    static void var(Variant const&);
    template <typename U, typename = decltype(
              var(U::toVariant(std::declval<U>())))>
    static std::true_type test(Type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(type_c<T>))();
};


struct HasToVariantT {
    template <typename T>
    constexpr auto operator()(Type<T> const&) const {
        return HasToVariantImpl<std::decay_t<T>>::value;
    }
};


///
/// Tests if type has Variant T::toVariant(T)
///
constexpr HasToVariantT hasToVariant;


struct ToVariantT {
    template <typename ...Args>
    auto operator()(Args&&... args) const;
};


///
/// Convinient shortcut function
///
constexpr ToVariantT toVariant;


template <typename T>
struct HasFromVariantImpl {
    static void var(T const&);
    template <typename U, typename = decltype(
              var(U::fromVariant(std::declval<Variant>())))>
    static std::true_type test(Type<U> const&);
    static std::false_type test(...);
    static constexpr auto const value = decltype(test(type_c<T>))();
};


struct HasFromVariantT {
    template <typename T>
    constexpr auto operator()(Type<T> const&) const {
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
        static_assert(DependentFalse<T>::value,
                      "No conversion to Variant is provided");
        return {};
    }
};


///
/// Specialization for types with `static Variant T::toVariant(T)`
///
template <typename T>
struct ToVariantImpl<T, When<hasToVariant(type_c<T>)>> {
    static Variant apply(T const& x) { return T::toVariant(x); }
};


///
/// Specialization for Variant build-in supported types
///
template <typename T>
struct ToVariantImpl<T, When<Variant::Types::convertible<T>()>> {
    static Variant apply(T x) { return Variant(x); }
};


///
/// Specialization for map types
///
template <typename T>
struct ToVariantImpl<T,
        When<isContainer(type_c<T>) &&
             isKeyValue(type_c<typename T::value_type>)>> {
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
/// Specialization for collection types
///
template <typename T>
struct ToVariantImpl<T,
        When<isContainer(type_c<T>) &&
             !isKeyValue(type_c<typename T::value_type>)>> {
    static Variant apply(T const& vec) {
        VariantVec ret;
        for (auto const& x: vec) {
            ret.push_back(ToVariantImpl<std::decay_t<decltype(x)>>::apply(x));
        }
        return Variant(ret);
    }
};


#if SERIALIZE_ENABLE_TYPE_SAFE
///
/// Specialization for `type_safe::strong_typedef`
///
template <typename T>
struct ToVariantImpl<T, When<
        !hasToVariant(type_c<T>) && strongTypeDef(type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = type_safe::underlying_type<T>;
        return ToVariantImpl<U>::apply(static_cast<U const&>(st));
    }
};


///
/// Specialization for `type_safe::constrained_type`
///
template <typename T>
struct ToVariantImpl<T, When<constrainedType(type_c<T>)>> {
    static Variant apply(T const& st) {
        return ToVariantImpl<std::decay_t<typename T::value_type>>::apply(
                    st.get_value());
    }
};


///
/// Specialization for `type_safe::integer`
///
template <typename T>
struct ToVariantImpl<T, When<integerType(type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::integer_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};


///
/// Specialization for `type_safe::floating_point`
///
template <typename T>
struct ToVariantImpl<T, When<floatingPoint(type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = typename T::floating_point_type;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};


///
/// Specialization for `type_safe::boolean`
///
template <typename T>
struct ToVariantImpl<T, When<boolean(type_c<T>)>> {
    static Variant apply(T const& st) {
        using U = bool;
        return ToVariantImpl<U>::apply(static_cast<U>(st));
    }
};
#endif


template <typename T>
struct HanaMapImpl {
    template <typename U,
              typename = std::enable_if_t<
                  boost::hana::is_a<boost::hana::map_tag, U>>>
    constexpr static bool isMap() {
        constexpr auto is_str = [](auto x) {
            return boost::hana::is_a<boost::hana::string_tag>(x);
        };

        using All = decltype(boost::hana::all_of(
            boost::hana::keys(std::declval<U>()), is_str));

        return boost::hana::value<All>();
    }

    template <typename U, typename = std::enable_if_t<isMap<U>()>>
    static std::true_type test(U&&);
    static std::false_type test(...);
    static constexpr auto value = decltype(test(std::declval<T>()))();
};


struct HanaMapT {
    template <typename T>
    constexpr bool operator()(Type<T> const&) const {
        return HanaMapImpl<std::decay_t<T>>::value;
    }
};


constexpr HanaMapT hanaMap;


template <typename T>
struct ToVariantImpl<T, When<hanaMap(type_c<T>)>> {
    static Variant apply(T const& map) {
        Variant::Map ret;
        boost::hana::for_each(map, boost::hana::fuse([&ret](auto key, auto value) {
            ret[boost::hana::to<char const*>(key)] = toVariant(value);
        }));
        return Variant(ret);
    }
};


template <typename ...Args>
auto ToVariantT::operator()(Args&&... args) const {
    return ToVariantImpl<std::decay_t<Args>...>::apply(
                std::forward<Args>(args)...);
}


template <typename T>
struct FromVariantT {
    auto operator()(Variant const& x) const;
};


///
/// Convinient shortcut function
///
template <typename T>
constexpr FromVariantT<T> fromVariant;


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
        static_assert(DependentFalse<T>::value,
                      "No conversion from Variant is provided");
        return {};
    }
};


///
/// Specialization for types with `static T T::fromVariant(Variant)`
///
template <typename T>
struct FromVariantImpl<T, When<hasFromVariant(type_c<T>)>> {
    static T apply(Variant const& x) { return T::fromVariant(x); }
};


///
/// Specialization for types for which Variant have conversion
///
template <typename T>
struct FromVariantImpl<T, When<Variant::Types::anyOf<T>()>> {
    static T apply(Variant const& x) { return static_cast<T>(x); }
};


///
/// Specialization for collection types
///
template <typename T>
struct FromVariantImpl<T, When<isContainer(type_c<T>) &&
        !isKeyValue(type_c<typename T::value_type>)>> {
    static T apply(Variant const& var) {
        T ret;
        for (auto const& x: var.vec()) {
            ret.push_back(FromVariantImpl<typename T::value_type>::apply(x));
        }
        return ret;
    }
};


///
/// Specialization for map types
///
template <typename T>
struct FromVariantImpl<T, When<isContainer(type_c<T>) &&
        isKeyValue(type_c<typename T::value_type>)>> {
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


#if SERIALIZE_ENABLE_TYPE_SAFE
///
/// Specialization for `type_safe::strong_typedef`
///
template <typename T>
struct FromVariantImpl<T, When<
        !hasFromVariant(type_c<T>) && strongTypeDef(type_c<T>)>> {
    static T apply(Variant const& var) {
        using U = type_safe::underlying_type<T>;
        return static_cast<T>(FromVariantImpl<U>::apply(var));
    }
};


///
/// Specialization for `type_safe::constrained_type`
///
template <typename T>
struct FromVariantImpl<T, When<constrainedType(type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<std::decay_t<typename T::value_type>>::apply(var));
    }
};


///
/// Specialization for `type_safe::integer`
///
template <typename T>
struct FromVariantImpl<T, When<integerType(type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::integer_type>::apply(var));
    }
};


///
/// Specialization for `type_safe::floating_point`
///
template <typename T>
struct FromVariantImpl<T, When<floatingPoint(type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<typename T::floating_point_type>::apply(var));
    }
};


///
/// Specialization for `type_safe::boolean`
///
template <typename T>
struct FromVariantImpl<T, When<boolean(type_c<T>)>> {
    static T apply(Variant const& var) {
        return T(FromVariantImpl<bool>::apply(var));
    }
};
#endif


template <typename T>
struct FromVariantImpl<T, When<hanaMap(type_c<T>)>> {
    static T apply(Variant const& var) {
        auto const map = var.map();
        T ret;
        boost::hana::for_each(ret,
                              boost::hana::fuse([&](auto key, auto& value) {
            using namespace std::string_literals;
            auto const it = map.find(boost::hana::to<char const*>(key));
            if (map.end() == it) {
                throw std::logic_error(boost::hana::to<char const*>(key) +
                                       " not found in map"s);
            }
            value = fromVariant<decltype(value)>(it->second);
        }));
        return ret;
    }
};


template <typename T>
struct FromVariantImpl<T, When<boost::hana::Constant<T>().value>> {
    static T apply(Variant const& var) {
        auto const tmp = fromVariant<typename T::value_type>(var);
        if (tmp != T::value) { throw VariantBadType(); }
        return T();
    }
};


template <typename T>
auto FromVariantT<T>::operator()(Variant const& x) const {
    return FromVariantImpl<std::decay_t<T>>::apply(x);
}


}
