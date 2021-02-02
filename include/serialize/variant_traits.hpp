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

#include <serialize/meta.hpp>
#include <serialize/variant.hpp>
#include <serialize/variant_conversion.hpp>

#include <boost/hana.hpp>

#include <type_traits>

namespace serialize {

/// \defgroup group-traits-auto-variant Variant conversion
/// \ingroup group-traits-opt-in
/// `Variant` conversion for user defined struct-like types

/// \defgroup group-traits-auto-variant-policy Policy
/// \ingroup group-traits-auto-variant
/// The traits' settings

/// \defgroup group-traits-auto-variant-meta Meta
/// \ingroup group-traits-auto-variant
/// Metadata that might be used by the traits.

/// Provide names for members in the traits that support renaming.
/// \ingroup group-traits-auto-variant-meta
///
/// Given:
/// * \code
/// struct T {
///     // meta
///     static auto names() {
///         return hana::make_map(
///                 hana::make_pair("x"_s, "y")
///         );
///     }
///     // member
///     int x;
/// };
/// \endcode
/// * trait `X`;
///
/// If `T` supports trait `X`, and trait `X` supports `names()`, then trait `X`
/// should use name "y" for member `x` in serialized Variant representation.
#ifdef SERIALIZE_DOXYGEN_INVOKED
constexpr auto names = [] { ... };
#endif

/// Provide default values for members in the traits that support defaulting.
/// \ingroup group-traits-auto-variant-meta
///
/// Given:
/// * \code
/// struct T {
///     // meta
///     static auto defaults() {
///         return hana::make_map(
///                 hana::make_pair("x"_s, 1)
///         );
///     }
///     // member
///     int x;
/// };
/// \endcode
/// * trait `X`;
///
/// If `T` supports trait `X`, and trait `X` supports `defaults()`, then trait `X`
/// should specify value `1` for member `x` during `Variant` to `T` conversion in case
/// if input Variant have no value for `x`.
#ifdef SERIALIZE_DOXYGEN_INVOKED
constexpr auto defaults = [] { ... };
#endif

namespace trait {
namespace detail {

/// \ingroup group-details
/// Has member `updateVar(serialize::Variant const&)`.
constexpr auto const hasUpdateVar = boost::hana::is_valid(
        [](auto t) -> decltype((void)std::declval<typename decltype(t)::type>().updateVar(
                           std::declval<Variant>())) {});

/// %Default implementation of `VarPolicy::Defaults` policy parameter
/// \ingroup group-traits-auto-variant-policy
struct Default {
    /// Does a type has a member function `static auto defaults()`
    static constexpr auto const has = boost::hana::is_valid(
        [](auto t) -> decltype((void)decltype(t)::type::defaults()) {
        });

    /// Does the member `name` has a default value in the class `C`
    template <typename T, typename S>
    static constexpr bool hasValue(boost::hana::basic_type<T>, S name) {
        using Found = decltype(
            name ^ boost::hana::in ^ boost::hana::keys(T::defaults()));
        return boost::hana::value<Found>();
    }

    /// %Default value for `name`
    template <class T, class S>
    static auto value(boost::hana::basic_type<T>, S name) {
        return T::defaults()[name];
    }
};

/// Default implementation of `VarPolicy::rename()` policy parameter
/// \ingroup group-traits-auto-variant-policy
struct Rename {
    // Does a type has a static member function `names()`
    static constexpr auto const hasNames = boost::hana::is_valid(
        [](auto t) -> decltype((void)decltype(t)::type::names()) {
        });

    // Is `name` is present in `names()` of `T`
    template <typename T, typename S>
    static constexpr bool presentInNames(S name) {
        using Found = decltype(
            name ^ boost::hana::in ^ boost::hana::keys(T::names()));
        return boost::hana::value<Found>();
    }

    // Does the member `name` has a name value in the class `C`
    template <typename T, typename S>
    static constexpr bool hasNameValue(S name) {
        if constexpr (hasNames(boost::hana::type_c<T>)) {
            return presentInNames<T>(name);
        } else {
            (void)name;
            return false;
        }
    }

    template <typename T, typename S>
    static char const* rename(S name) {
        if constexpr (hasNameValue<T>(name)) {
            return T::names()[name];
        } else {
            return boost::hana::to<char const*>(name);
        }
    }

    /// \return name from map returned by `T::names()` static member function,
    /// if present, else identity
    template <class T, class S>
    char const* operator()(boost::hana::basic_type<T>, S name) const noexcept {
        return rename<T>(name);
    }
};

template <typename T, typename F = decltype(toVariant2)>
void toVariantWrap(Variant& var, T&& val, F const& to_variant = toVariant2) {
    to_variant(var, std::forward<T>(val));
}

template <typename T, typename S, typename F = decltype(fromVariant2)>
void fromVariantWrap(T& val,
                     Variant const& var,
                     S const& name,
                     F const& from_variant = fromVariant2) {
    try {
        return from_variant(val, var);
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

/// Configuration for `Var`
/// \ingroup group-traits-auto-variant-policy
struct VarPolicy {
    /// treat missing keys as empty containers
    static auto constexpr empty_container_not_required = false;

    /// serialize a value even if it has default value
    static auto constexpr serialize_default_value = true;

    /// allow extra properties in `Variant`
    static auto constexpr allow_additional_properties = true;

    /// from variant conversion functional object
    static constexpr auto from_variant = fromVariant2;

    /// to variant conversion function object
    static constexpr auto to_variant = toVariant2;

    /// rename function object
    static constexpr detail::Rename rename{};

    /// defaults policy
    using Defaults = detail::Default;

    struct NoTag {};
    /// additional property '__tag' with the value `tag` in `Variant`
    static constexpr NoTag tag{};
};

using VarDefPolicy [[deprecated("Use serialize::trait::VarPolicy.")]] = VarPolicy;

/// Convert `x` to `Variant`
/// \ingroup group-traits-auto-variant
///
/// Conversion can be customized via `Policy`.
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
template <class T, class Policy = VarPolicy>
Variant toVariantImpl(T const& x) {
    Variant::Map ret;

    boost::hana::for_each(
            x, boost::hana::fuse([&](auto name, auto value) {
                auto const renamed = Policy::rename(boost::hana::type_c<T>, name);
                if constexpr (isOptional(boost::hana::type_c<decltype(value)>)) {
                    if (value.has_value()) {
                        detail::toVariantWrap(ret[renamed], *value, Policy::to_variant);
                    }
                } else {
                    if constexpr (Policy::Defaults::has(boost::hana::type_c<T>)) {
                        if constexpr (!Policy::serialize_default_value
                                      && Policy::Defaults::hasValue(
                                              boost::hana::type_c<T>, name)) {
                            static_assert(
                                    std::is_convertible_v<
                                            decltype(Policy::Defaults::value(
                                                    boost::hana::type_c<T>, name)),
                                            std::remove_reference_t<decltype(value)>>,
                                    "Default value should be convertible to field "
                                    "type");
                            if (Policy::Defaults::value(boost::hana::type_c<T>, name)
                                == value) {
                                return;
                            }
                        }
                    }

                    if constexpr (isContainer(boost::hana::type_c<decltype(value)>)) {
                        if constexpr (!Policy::empty_container_not_required) {
                            detail::toVariantWrap(
                                    ret[renamed], value, Policy::to_variant);
                        } else if (begin(value) != end(value)) {
                            detail::toVariantWrap(
                                    ret[renamed], value, Policy::to_variant);
                        }
                    } else {
                        detail::toVariantWrap(ret[renamed], value, Policy::to_variant);
                    }
                }
            }));

    if constexpr (!std::is_same_v<std::remove_const_t<decltype(Policy::tag)>,
                                  typename Policy::NoTag>) {
        detail::toVariantWrap(ret["__tag"], Policy::tag, Policy::to_variant);
    }

    return Variant(ret);
}

/// Convert `x` to `T`
/// \ingroup group-traits-auto-variant
///
/// Conversion can be customized via `Policy`.
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
template <class T, class Policy = VarPolicy>
T fromVariantImpl(serialize::Variant const& x) {
    using namespace std::literals;
    T ret;

    using MapT = std::conditional_t<Policy::allow_additional_properties,
                                    decltype(x.map()),
                                    std::decay_t<decltype(x.map())>>;
    MapT map = x.map();

    if constexpr (!std::is_same_v<std::remove_const_t<decltype(Policy::tag)>,
                                  typename Policy::NoTag>) {
        std::remove_const_t<decltype(Policy::tag)> tmp;
        auto const it = map.find("__tag");
        if (it == map.end()) {
            throw std::logic_error("'__tag' is required"s);
        }
        detail::fromVariantWrap(tmp, it->second, "__tag", Policy::from_variant);
        if constexpr (!Policy::allow_additional_properties) {
            map.erase(it);
        }
    }

    boost::hana::for_each(
            boost::hana::accessors<T>(), boost::hana::fuse([&](auto name, auto value) {
                auto const renamed = Policy::rename(boost::hana::type_c<T>, name);
                auto& tmp = value(ret);
                auto const it = map.find(renamed);

                if (map.end() == it) {
                    if constexpr (Policy::Defaults::has(boost::hana::type_c<T>)) {
                        if constexpr (Policy::Defaults::hasValue(boost::hana::type_c<T>,
                                                                 name)) {
                            static_assert(std::is_convertible_v<
                                                  decltype(Policy::Defaults::value(
                                                          boost::hana::type_c<T>, name)),
                                                  std::remove_reference_t<decltype(
                                                          value(std::declval<T>()))>>,
                                          "Default value should be convertible to field "
                                          "type");
                            tmp = Policy::Defaults::value(boost::hana::type_c<T>, name);
                            return;
                        }
                    }

                    if constexpr (!isOptional(boost::hana::type_c<decltype(tmp)>)
                                  && ((isContainer(boost::hana::type_c<decltype(tmp)>)
                                       && !Policy::empty_container_not_required)
                                      || !isContainer(
                                              boost::hana::type_c<decltype(tmp)>))) {
                        throw std::logic_error("'"s + renamed + "' is required"s);
                    }

                } else {
                    if constexpr (isOptional(boost::hana::type_c<decltype(tmp)>)) {
                        std::remove_reference_t<decltype(*tmp)> under;
                        detail::fromVariantWrap(
                                under, it->second, renamed, Policy::from_variant);
                        tmp = std::move(under);
                    } else {
                        detail::fromVariantWrap(
                                tmp, it->second, renamed, Policy::from_variant);
                    }

                    if constexpr (!Policy::allow_additional_properties) {
                        map.erase(it);
                    }
                }
            }));

    if constexpr (!Policy::allow_additional_properties) {
        if (!map.empty()) {
            throw std::logic_error("'" + map.begin()->first + "' is unknown");
        }
    }

    return ret;
}

/// Adds conversion support to and from `Variant`
/// \ingroup group-traits-auto-variant
///
/// Specifically adds members:
/// * `static Variant toVariant(Derived const&)`
/// * `static Derived fromVariant(Variant const&)`
///
/// Supports
/// * `names()`;
/// * `defaults()`.
///
/// \pre `Derived` should be a Boost.Hana.Struct.
template <typename Derived, class Policy = VarPolicy>
struct Var {
    static Variant toVariant(Derived const& x) {
        return toVariantImpl<Derived, Policy>(x);
    }

    static Derived fromVariant(Variant const& x) {
        return fromVariantImpl<Derived, Policy>(x);
    }

protected:
    ~Var() = default;
};

template <typename Derived, typename Policy = VarPolicy>
using VarDef [[deprecated("Use serialize::trait::Var.")]] = Var<Derived, Policy>;

/// Updates the specified fields
/// \ingroup group-traits-auto-variant
///
/// Conversion can be customized via `Policy`.
///
/// `T` can provide
/// * names().
///
/// \pre `T` should be a Boost.Hana.Struct.
template <class T, class Policy = VarPolicy>
void updateVarImpl(T& self, Variant const& x) {
    auto const& map = x.map();
    for (auto const& v : map) {
        bool found{false};
        boost::hana::for_each(
                boost::hana::accessors<T>(),
                boost::hana::fuse([&](auto name, auto value) {
                    auto const renamed = Policy::rename(boost::hana::type_c<T>, name);
                    if (renamed != v.first) {
                        return;
                    }
                    auto& tmp = value(self);
                    if constexpr (detail::hasUpdateVar(
                                          boost::hana::type_c<std::remove_reference_t<
                                                  decltype(tmp)>>)) {
                        tmp.updateVar(v.second);
                    } else {
                        detail::fromVariantWrap<decltype(tmp)>(tmp, v.second, renamed);
                    }
                    found = true;
                }));

        if constexpr (!Policy::allow_additional_properties) {
            if (!found) {
                throw std::logic_error("'" + v.first + "'" + " is unknown");
            }
        }
    }
}

/// Adds member `void updateVar(Variant const&)`
/// \ingroup group-traits-auto-variant
///
/// Updates the specified fields.
///
/// Supports
/// * names().
///
/// \pre `Derived` should be a Boost.Hana.Struct.
template <typename Derived, typename Policy = VarPolicy>
struct UpdateFromVar {
    void updateVar(Variant const& x) {
        updateVarImpl<Derived, Policy>(static_cast<Derived&>(*this), x);
    }

protected:
    ~UpdateFromVar() = default;
};

/// Updates the specified fields.
/// \ingroup group-traits-auto-variant
/// \pre `Derived` should be a Boost.Hana.Struct.
template <class T, class Opt>
void updateOptImpl(T& self, Opt const& x) {
    boost::hana::for_each(
            x, boost::hana::fuse([&](auto rkey, auto rvalue) {
                auto found = boost::hana::to_map(boost::hana::accessors<T>())[rkey];
                if constexpr (isOptional(boost::hana::type_c<decltype(rvalue)>)) {
                    if (rvalue) {
                        found(self) = *rvalue;
                    }
                } else {
                    found(self) = rvalue;
                }
            }));
}

/// Adds member `void updateOpt(Opt const&)`
/// \ingroup group-traits-auto-variant
///
/// Updates the specified fields.
///
/// \pre `Derived` should be a Boost.Hana.Struct.
template <typename Derived, typename Opt>
struct UpdateFromOpt {
    void updateOpt(Opt const& x) {
        updateOptImpl(static_cast<Derived&>(*this), x);
    }

protected:
    ~UpdateFromOpt() = default;
};

} // namespace trait
} // namespace serialize

/// Enables to `serialize::Variant` conversion for `T`
/// \ingroup group-traits-auto-variant
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_TO_VARIANT(T)                                                          \
    static serialize::Variant toVariant(T const& x) {                                    \
        return serialize::trait::toVariantImpl<T>(x);                                    \
    }

/// Enables to `serialize::Variant` conversion for `T`
/// \ingroup group-traits-auto-variant
///
/// Conversion can be customized via `Policy`.
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_TO_VARIANT_P(T, Policy)                                                \
    static serialize::Variant toVariant(T const& x) {                                    \
        return serialize::trait::toVariantImpl<T, Policy>(x);                            \
    }

/// Enables from `serialize::Variant` conversion for `T`
/// \ingroup group-traits-auto-variant
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_FROM_VARIANT(T)                                                        \
    static T fromVariant(serialize::Variant const& x) {                                  \
        return serialize::trait::fromVariantImpl<T>(x);                                  \
    }

/// Enables from `serialize::Variant` conversion for `T`
/// \ingroup group-traits-auto-variant
///
/// Conversion can be customized via `Policy`.
///
/// `T` can provide
/// * `names()`;
/// * `defaults()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_FROM_VARIANT_P(T, Policy)                                              \
    static T fromVariant(serialize::Variant const& x) {                                  \
        return serialize::trait::fromVariantImpl<T, Policy>(x);                          \
    }

/// Enables from `serialize::Variant` update for `T`
/// \ingroup group-traits-auto-variant
///
/// `T` can provide
/// * `names()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_UPDATE_FROM_VARIANT(T)                                                 \
    void updateVar(serialize::Variant const& x) {                                        \
        serialize::trait::updateVarImpl<T>(*this, x);                                    \
    }

/// Enables from `serialize::Variant` update for `T`
/// \ingroup group-traits-auto-variant
///
/// Updating can be customized via `Policy`.
///
/// `T` can provide
/// * `names()`.
///
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_UPDATE_FROM_VARIANT_P(T, Policy)                                       \
    void updateVar(serialize::Variant const& x) {                                        \
        serialize::trait::updateVarImpl<T, Policy>(*this, x);                            \
    }

/// Enables from `Opt` update for `T`
/// \ingroup group-traits-auto-variant
/// \pre `T` should be a Boost.Hana.Struct.
#define SERIALIZE_UPDATE_FROM_OPT(T, Opt)                                                \
    void updateOpt(Opt const& x) {                                                       \
        serialize::trait::updateOptImpl(*this, x);                                       \
    }
