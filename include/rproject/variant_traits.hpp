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
#include <rproject/meta.hpp>
#include <rproject/variant.hpp>
#include <rproject/variant_conversion.hpp>

// boost
#include <boost/hana.hpp>

// std
#include <type_traits>


///
/// \file variant_traits.hpp
/// Some add-ons for enabling to and from Variant conversion
///


namespace trait {


///
/// Specify stub for default value (means no default value specified)
///
struct NoDefault {
    BOOST_HANA_DEFINE_STRUCT(NoDefault);
};


namespace detail {


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


template <typename T>
auto toVariantWrap(T&& x) {
    return toVariant(std::forward<T>(x));
}


template <typename T>
auto fromVariantWrap(Variant const& x) {
    return fromVariant<T>(x);
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
            ret[boost::hana::to<char const*>(name)] =
                    detail::toVariantWrap(value);
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
                tmp = detail::fromVariantWrap<
                        std::decay_t<decltype(tmp)>>(it->second);
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
                            detail::toVariantWrap(*value);
                }
            } else {
                ret[boost::hana::to<char const*>(name)] =
                        detail::toVariantWrap(value);
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
                    tmp = detail::fromVariantWrap<
                            std::decay_t<decltype(*tmp)>>(it->second);
                } else {
                    tmp = detail::fromVariantWrap<
                            std::decay_t<decltype(tmp)>>(it->second);
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
                tmp = detail::fromVariantWrap<
                        std::decay_t<decltype(tmp)>>(v.second);
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
