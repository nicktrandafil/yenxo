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


#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL


// local
#include <meta.hpp>
#include <variant.hpp>
#include <when.hpp>

// boost
#include <boost/hana.hpp>

// std
#include <type_traits>


///
/// \file variant_mixins.hpp
/// Some add-ons for enabling to and from Variant conversion
///


namespace mixin {


///
/// Specify stub for default value (means no default value specified)
///
struct NoDefault {
    BOOST_HANA_DEFINE_STRUCT(NoDefault);
};


namespace detail {


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
/// Specialization from `Container`
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
/// Specialization from `Dict`
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


///
/// Does a type has a static member `default_mem_vals`
///
constexpr auto const hasDefaultMemVals = boost::hana::is_valid(
            [](auto t) -> decltype((void) decltype(t)::type::default_mem_vals) {
});


///
/// Is `name` is present in `default_mem_vals` of `T`
///
template <typename T, typename S>
constexpr bool present(S name) {
    using Found = decltype(
        name ^boost::hana::in^ boost::hana::keys(T::default_mem_vals));
    return boost::hana::value<Found>();
}


///
/// Does type of provided entry in `default_mem_vals` for `name` is `NoDefault`
///
template <typename T, typename S>
constexpr bool noDefault(S name) {
    return std::is_same_v<std::decay_t<decltype(T::default_mem_vals[name])>,
                          NoDefault>;
}


///
/// Does the field `name` has a default value in the class `C`
///
template <typename T, typename S>
constexpr bool hasDefaultValue(S name) {
    if constexpr(present<T>(name)) {
        return !noDefault<T>(name);
    } else {
        return false;
    }
}


template <typename T>
constexpr void checkOrphanKeys(rp::Type<T> const&) {
    constexpr decltype(boost::hana::keys(T::default_mem_vals)) keys;
    boost::hana::for_each(
        keys,
        [](auto key) {
            constexpr decltype(boost::hana::keys(T())) keys;
            BOOST_HANA_CONSTANT_ASSERT_MSG(
                key ^boost::hana::in^ keys,
                "There are unknown fields in default_mem_vals");
        });
}


} // namespace detail


///
/// Adds conversion support to and from `Variant`
/// Specifically, adds methods:
///     `static Variant toVariant(Derived)`
///     `static Derived fromVariant(Variant)`
///
/// The methods does not deal with optional types and default values. Therefore,
/// for the method `fromVariant` all members must be presented in a variant,
/// and, for the method `toVariant` all fields will be serialized into a variant
///
template <typename Derived>
struct Var {
    static Variant toVariant(Derived const& x) {
        Variant::Map ret;

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
            ret[boost::hana::to<char const*>(name)] = detail::toVariant(value);
        }));

        return Variant(ret);
    }

    static Derived fromVariant(Variant const& x) {
        using namespace std::literals;
        Derived ret;
        auto const& map = x.map();

        boost::hana::for_each(boost::hana::accessors<Derived>(),
                       boost::hana::fuse([&](auto name, auto value) {
            auto& tmp = value(ret);
            auto const it = map.find(boost::hana::to<char const*>(name));
            if (map.end() == it) {
                throw std::logic_error(
                            boost::hana::to<char const*>(name) +
                            " not found in map"s);
            } else {
                detail::fromVariant(tmp, it->second);
            }
        }));

        return ret;
    }

protected:
    ~Var() = default;
};


///
/// Adds conversion support to and from `Variant`, defaulting missings fields
/// Specifically adds members:
///     `static Variant toVariant(Derived)`
/// 	`static Derived fromVariant(Variant)`
///
/// Requires a static member `default_mem_vals` of hana map to be presented in
/// `Derived`, where the kays are member names and the values are convertible
/// to the corresponding members.
///
template <typename Derived>
struct VarDef {
    static Variant toVariant(Derived const& x) {
        Variant::Map ret;

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
            if constexpr (rp::isOptional(rp::type_c<decltype(value)>)) {
                if (value.has_value()) {
                    ret[boost::hana::to<char const*>(name)] =
                            detail::toVariant(*value);
                }
            } else {
                ret[boost::hana::to<char const*>(name)] =
                        detail::toVariant(value);
            }
        }));

        return Variant(ret);
    }

    static Derived fromVariant(Variant const& x) {
        using namespace std::literals;
        using namespace boost::hana::literals;

        static_assert(
            detail::hasDefaultMemVals(boost::hana::type_c<Derived>),
            "The T must have `defail_mem_vals` static member");
        detail::checkOrphanKeys(rp::type_c<Derived>);

        Derived ret;
        auto const& map = x.map();

        boost::hana::for_each(boost::hana::accessors<Derived>(),
                       boost::hana::fuse([&](auto name, auto value) {
            auto& tmp = value(ret);
            auto const it = map.find(boost::hana::to<char const*>(name));

            if (map.end() == it) {
                if constexpr (detail::hasDefaultValue<Derived>(name)) {
                    BOOST_HANA_CONSTEXPR_ASSERT_MSG(
                        (std::is_convertible_v<
                            std::decay_t<decltype(Derived::default_mem_vals[name])>,
                            std::decay_t<decltype(value(std::declval<Derived>()))>>),
                        "The provided default type in"_s +
                        " default_mem_vals for "_s + name +
                        " does not match with the actual type"_s);

                    tmp = Derived::default_mem_vals[name];
                } else if constexpr (
                            !rp::isOptional(rp::type_c<decltype(tmp)>)) {
                    throw std::logic_error(
                                boost::hana::to<char const*>(name) +
                                " not found in map, and default"
                                " value is not provided"s);
                }

            } else {
                if constexpr (rp::isOptional(rp::type_c<decltype(tmp)>)) {
                    tmp = detail::fromVariant.operator()<decltype(*tmp)>(it->second);
                } else {
                    detail::fromVariant(tmp, it->second);
                }
            }
        }));

        return ret;
    }

protected:
    ~VarDef() = default;
};


///
/// Adds conversion support to and from `Variant`, defaulting missings fields
/// Specifically adds members:
///     `static Variant toVariant(Derived)`
/// 	`static Derived fromVariant(Variant)`
///
/// Requires a static member `default_mem_vals` of hana map to be presented in
/// `Derived`, where the kays are member names and the values are convertible
/// to the corresponding members.
///
/// Ensures:
/// 	- default value presented for every member (use `NoDefault` as stub);
/// 	- types of default values are convertible to the correspondig members;
/// 	- there are no unknown fields in the dict
///
template <typename Derived>
struct VarDefExplicit : private VarDef<Derived> {
    static Derived fromVariant(Variant const& x) {
        check();
        return VarDef<Derived>::fromVariant(x);
    }

    static Variant toVariant(Derived const& x) {
        check();
        return VarDef<Derived>::toVariant(x);
    }

    static constexpr void check() {
        using namespace boost::hana::literals;

        static_assert(
                detail::hasDefaultMemVals(boost::hana::type_c<Derived>),
                "The T must have `defail_mem_vals` static member");

        detail::checkOrphanKeys(rp::type_c<Derived>);

        boost::hana::for_each(
            boost::hana::accessors<Derived>(),
            boost::hana::fuse([](auto name, auto) {
                if constexpr (!detail::present<Derived>(name)) {
                    BOOST_HANA_CONSTEXPR_ASSERT_MSG(
                        rp::DependentFalse<Derived>::value,
                        name +
                        " not present in default_mem_vals"_s);
                }
            }));
    }

protected:
    ~VarDefExplicit() = default;
};


///
/// Adds member `void update(Variant)`
/// Updates the specified fields
///
template <typename Derived>
struct UpdateFromVar {
    void update(Variant const& x) {
        Derived& self = static_cast<Derived&>(*this);
        auto const& map = x.map();
        for (auto const& v: map) {
            bool found{false};
            boost::hana::for_each(boost::hana::accessors<Derived>(),
                                  boost::hana::fuse([&](auto name, auto value) {
                if (boost::hana::to<char const*>(name) != v.first) { return; }
                auto& tmp = value(self);
                detail::fromVariant(tmp, v.second);
                found = true;
            }));

            if (!found) {
                throw std::logic_error(v.first + " no such member");
            }
        }
    }

protected:
    ~UpdateFromVar() = default;
};


} // namespace
