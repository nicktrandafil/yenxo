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
#include <serialize/meta.hpp>
#include <serialize/variant.hpp>
#include <serialize/variant_conversion.hpp>

// boost
#include <boost/hana.hpp>

// std
#include <type_traits>


/// \file variant_traits.hpp
/// Some add-ons for enabling to and from Variant conversion


namespace serialize::trait {


/// Specify stub for default value (means no default value specified)
struct NoDefault {
    BOOST_HANA_DEFINE_STRUCT(NoDefault);
};


namespace detail {


/// Does a type has a static member function `defaults()`
constexpr auto const hasDefaults = boost::hana::is_valid(
            [](auto t) -> decltype((void) decltype(t)::type::defaults()) {
});


/// Does a type has a static member function `names()`
constexpr auto const hasNames = boost::hana::is_valid(
            [](auto t) -> decltype((void) decltype(t)::type::names()) {
});


/// Is the type a container
constexpr auto const isContainer = boost::hana::is_valid(
  [](auto t) -> decltype((void) begin(std::declval<typename decltype(t)::type>())) {
});


/// Is `name` is present in `defaults()` of `T`
template <typename T, typename S>
constexpr bool presentInDefaults(S name) {
    using Found = decltype(
        name ^boost::hana::in^ boost::hana::keys(T::defaults()));
    return boost::hana::value<Found>();
}


/// Is `name` is present in `defaults()` of `T`
template <typename T, typename S>
constexpr bool presentInNames(S name) {
    using Found = decltype(
        name ^boost::hana::in^ boost::hana::keys(T::names()));
    return boost::hana::value<Found>();
}


/// Does type of provided entry in `defaults()` for `name` is `NoDefault`
template <typename T, typename S>
constexpr bool noDefault(S name) {
    return std::is_same_v<std::decay_t<decltype(T::defaults()[name])>,
                          NoDefault>;
}


/// Does the field `name` has a default value in the class `C`
template <typename T, typename S>
constexpr bool hasDefaultValue(S name) {
    if constexpr (hasDefaults(boost::hana::type_c<T>)) {
        if constexpr(presentInDefaults<T>(name)) {
            return !noDefault<T>(name);
        } else {
            return false;
        }
    } else {
        (void) name;
        return false;
    }
}


/// Does the field `name` has a name value in the class `C`
template <typename T, typename S>
constexpr bool hasNameValue(S name) {
    if constexpr (hasNames(boost::hana::type_c<T>)) {
        return presentInNames<T>(name);
    } else {
        (void) name;
        return false;
    }
}


template <typename T, typename S>
auto rename(S name) {
    if constexpr (hasNameValue<T>(name)) {
        return T::names()[name];
    } else {
        return boost::hana::to<char const*>(name);
    }
}


template <typename T>
constexpr void checkOrphanKeysInDefaults(Type<T> const&) {
    boost::hana::for_each(
        decltype(boost::hana::keys(T::defaults()))(),
        [](auto key) {
            BOOST_HANA_CONSTANT_ASSERT_MSG(
                key ^boost::hana::in^ decltype(boost::hana::keys(T()))(),
                "There are unknown fields in defaults()");
        });
}


template <typename T, typename F = decltype(toVariant)>
auto toVariantWrap(T&& x, F const& to_variant = toVariant) {
    return to_variant(std::forward<T>(x));
}


template <typename T, typename F = decltype(fromVariant<T>)>
auto fromVariantWrap(char const* name, Variant const& x,
                     F const& from_variant = fromVariant<T>) {
    try {
        return from_variant(x);
    } catch (serialize::VariantErr& e) {
        e.prependPath(name);
        throw;
    } catch (std::exception const& e) {
        VariantErr err(e.what());
        err.prependPath(name);
        throw std::move(err);
    }
}


} // namespace detail


/// Adds conversion support to and from `Variant`
/// Specifically, adds methods:
///     `static Variant toVariant(Derived)`
///     `static Derived fromVariant(Variant)`
///
/// The methods does not deal with optional types and default values. Therefore,
/// for the method `fromVariant` all members must be presented in a variant,
/// and, for the method `toVariant` all fields will be serialized into a variant
template <typename Derived>
struct Var {
    static Variant toVariant(Derived const& x) {
        Variant::Map ret;

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
            auto const renamed = detail::rename<Derived>(name);
            if constexpr (isOptional(type_c<decltype(value)>)) {
                if (value.has_value()) {
                    ret[renamed] = detail::toVariantWrap(*value);
                } else {
                    ret[renamed] = Variant();
                }
            } else {
                ret[renamed] = detail::toVariantWrap(value);
            }
        }));

        return Variant(ret);
    }

    static Derived fromVariant(Variant const& x) {
        using namespace std::literals;
        Derived ret;
        auto const& map = x.map();

        boost::hana::for_each(boost::hana::accessors<Derived>(),
                       boost::hana::fuse([&](auto name, auto value) {
            auto const renamed = detail::rename<Derived>(name);
            auto& tmp = value(ret);
            auto const it = map.find(renamed);
            if (map.end() == it) {
                throw std::logic_error(renamed + " not found in map"s);
            } else {
                if constexpr (isOptional(type_c<decltype(tmp)>)) {
                    tmp = detail::fromVariantWrap<decltype(*tmp)>(renamed, it->second);
                } else {
                    tmp = detail::fromVariantWrap<decltype(tmp)>(renamed, it->second);
                }
            }
        }));

        return ret;
    }

protected:
    ~Var() = default;
};


/// Configuaratoin for `VarDef`
struct VarDefPolicy {
    /// treat missing keys as empty containers
    static auto constexpr empty_container_not_required = false;

    /// serialize a value even if it has it's default value
    static auto constexpr serialize_default_value = true;

    /// from variant conversion functional object
    template <class T>
    static constexpr auto from_variant = fromVariant<T>;

    /// to variant conversion function object
    static constexpr auto to_variant = toVariant;
};


/// Adds conversion support to and from `Variant`, defaulting missings fields
/// Specifically adds members:
///     `static Variant toVariant(Derived)`
/// 	`static Derived fromVariant(Variant)`
///
/// Requires a static member function `defaults` of hana map to be presented in
/// `Derived`, where the kays are member names and the values are convertible
/// to the corresponding members.
template <typename Derived, class Policy = VarDefPolicy>
struct VarDef {
    static Variant toVariant(Derived const& x) {
        Variant::Map ret;

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
            auto const renamed = detail::rename<Derived>(name);
            if constexpr (isOptional(type_c<decltype(value)>)) {
                if (value.has_value()) {
                    ret[renamed] = detail::toVariantWrap(*value, Policy::to_variant);
                }
            } else {
                if constexpr (detail::hasDefaults(boost::hana::type_c<Derived>)) {
                    if constexpr (!Policy::serialize_default_value && detail::hasDefaultValue<Derived>(name)) {
                        static_assert(std::is_convertible_v<
                                std::decay_t<decltype(Derived::defaults()[name])>,
                                std::decay_t<decltype(value)>>,
                             "Default value should be convertible to field type");
                        if (Derived::defaults()[name] == value) { return; }
                    }
                }

                if constexpr(detail::isContainer(
                                 boost::hana::type_c<decltype(value)>)) {
                    if constexpr (!Policy::empty_container_not_required) {
                        ret[renamed] = detail::toVariantWrap(value, Policy::to_variant);
                    } else if (begin(value) != end(value)) {
                        ret[renamed] = detail::toVariantWrap(value, Policy::to_variant);
                    }
                } else {
                    ret[renamed] = detail::toVariantWrap(value, Policy::to_variant);
                }
            }
        }));

        return Variant(ret);
    }

    static Derived fromVariant(Variant const& x) {
        using namespace std::literals;

        Derived ret;
        auto const& map = x.map();

        boost::hana::for_each(boost::hana::accessors<Derived>(),
                       boost::hana::fuse([&](auto name, auto value) {
            auto const renamed = detail::rename<Derived>(name);
            auto& tmp = value(ret);
            auto const it = map.find(renamed);

            if (map.end() == it) {
                if constexpr (detail::hasDefaults(boost::hana::type_c<Derived>)) {
                    if constexpr (detail::hasDefaultValue<Derived>(name)) {
                        static_assert(std::is_convertible_v<
                                std::decay_t<decltype(Derived::defaults()[name])>,
                                std::decay_t<decltype(value(std::declval<Derived>()))>>,
                            "Default value should be convertible to field type");
                        tmp = Derived::defaults()[name];
                        return;
                    }
                }

                if constexpr (
                        !isOptional(type_c<decltype(tmp)>) &&
                        ((isContainer(type_c<decltype(tmp)>) &&
                            !Policy::empty_container_not_required) ||
                                !isContainer(type_c<decltype(tmp)>))) {
                    throw std::logic_error(renamed + " not found in map, and default value is not provided"s);
                }

            } else {
                if constexpr (isOptional(type_c<decltype(tmp)>)) {
                    tmp = detail::fromVariantWrap<decltype(*tmp)>(renamed, it->second, Policy::template from_variant<decltype(*tmp)>);
                } else {
                    tmp = detail::fromVariantWrap<decltype(tmp)>(renamed, it->second, Policy::template from_variant<decltype(tmp)>);
                }
            }
        }));

        return ret;
    }

protected:
    ~VarDef() = default;
};


/// Adds conversion support to and from `Variant`, defaulting missings fields
/// Specifically adds members:
///     `static Variant toVariant(Derived)`
/// 	`static Derived fromVariant(Variant)`
///
/// Requires a static member function `defaults()` of hana map to be presented
/// in `Derived`, where the kays are member names and the values are convertible
/// to the corresponding members.
///
/// Ensures:
/// 	- default value presented for every member (use `NoDefault` as stub);
/// 	- types of default values are convertible to the correspondig members;
/// 	- there are no unknown fields in the dict
template <typename Derived>
struct VarDefExplicit : private VarDef<Derived> {
    static Derived fromVariant(Variant const& x) {
        static_assert(check());
        return VarDef<Derived>::fromVariant(x);
    }

    static Variant toVariant(Derived const& x) {
        static_assert(check());
        return VarDef<Derived>::toVariant(x);
    }

    static constexpr bool check() {
        using namespace boost::hana::literals;

        static_assert(
                detail::hasDefaults(boost::hana::type_c<Derived>),
                "The T must have `defaults` static member function");

        detail::checkOrphanKeysInDefaults(type_c<Derived>);

        boost::hana::for_each(
            boost::hana::accessors<Derived>(),
            boost::hana::fuse([](auto name, auto) {
                BOOST_HANA_CONSTEXPR_ASSERT_MSG(
                    detail::presentInDefaults<Derived>(name),
                    (name + " not present in defaults()"_s).c_str());
            }));

        return true;
    }

protected:
    ~VarDefExplicit() = default;
};


/// Adds member `void update(Variant)`
/// Updates the specified fields
template <typename Derived>
struct UpdateFromVar {
    void updateVar(Variant const& x) {
        Derived& self = static_cast<Derived&>(*this);
        auto const& map = x.map();
        for (auto const& v: map) {
            bool found{false};
            boost::hana::for_each(boost::hana::accessors<Derived>(),
                                  boost::hana::fuse([&](auto name, auto value) {
                auto const renamed = detail::rename<Derived>(name);
                if (renamed != v.first) { return; }
                auto& tmp = value(self);
                tmp = detail::fromVariantWrap<decltype(tmp)>(renamed, v.second);
                found = true;
            }));

            if (!found) {
                throw std::logic_error("'" + v.first + "'" + " no such member");
            }
        }
    }

protected:
    ~UpdateFromVar() = default;
};


/// Adds member `void update(Opt const&)`
/// Updates the specified fields
template <typename Derived, typename Opt>
struct UpdateFromOpt {
    void updateOpt(Opt const& x) {
        Derived& self = static_cast<Derived&>(*this);
        boost::hana::for_each(x,
                              boost::hana::fuse([&](auto rkey, auto rvalue) {
            auto found = boost::hana::to_map(boost::hana::accessors<Derived>())[rkey];
            if constexpr (isOptional(type_c<decltype(rvalue)>)) {
                if (rvalue) { found(self) = *rvalue; }
            } else {
                found(self) = rvalue;
            }
        }));
    }

protected:
    ~UpdateFromOpt() = default;
};


} // namespace
