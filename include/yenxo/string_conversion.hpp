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

#include <yenxo/enum_traits.hpp>
#include <yenxo/exception.hpp>
#include <yenxo/meta.hpp>
#include <yenxo/type_name.hpp>

#include <boost/hana/for_each.hpp>

#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace yenxo {

/// To string conversion
///
/// \ingroup group-function
/// Converts the `type` to string
#ifdef YENXO_DOXYGEN_INVOKED
inline constexpr auto toString = [](auto&& type) { return type - dispatched; };
#else
// To string conversion implementation
template <typename T, typename = void>
struct ToStringImpl : ToStringImpl<T, When<true>> {};

// Fall back
template <typename T, bool condition>
struct ToStringImpl<T, When<condition>> {
    static std::string apply(T const&) {
        using Ret = typename T::is_not_convertible_to_string;
        return Ret{};
    }
};

// Conversion with `std::to_string`
template <typename T>
struct ToStringImpl<
        T,
        When<detail::Valid<decltype(std::to_string(std::declval<T>()))>::value>> {
    static std::string apply(T const& x) {
        return std::to_string(x);
    }
};

// Types explicitly covertible to string
template <typename T>
struct ToStringImpl<
        T,
        When<detail::Valid<decltype(std::string(std::declval<T>()))>::value>> {
    static std::string apply(T const& x) {
        return std::string(x);
    }
};

// Types with specialized EnumTraits
template <typename T>
struct ToStringImpl<T,
                    When<detail::Valid<decltype(EnumTraits<T>::toString(
                            std::declval<T>()))>::value>> {
    static char const* apply(T x) {
        return EnumTraits<T>::toString(x);
    }

    template <size_t I>
    static char const* apply(T x) {
        char const* ret = nullptr;
        size_t i = 0;

        boost::hana::for_each(EnumTraits<T>::strings(), [&](auto s) {
            if (EnumTraits<T>::values[i++] == x) {
                if constexpr (static_cast<size_t>(boost::hana::length(s)) > I) {
                    ret = boost::hana::at_c<I>(s);
                }
            }
        });

        if (ret == nullptr) {
            throw StringConversionError(
                    std::string("enum variant '") + apply(x)
                    + std::string("' doesn't have string representation ")
                    + std::to_string(I));
        }

        return ret;
    }
};

struct ToStringT {
    template <typename T>
    std::string operator()(T&& x) const {
        return ToStringImpl<std::decay_t<T>>::apply(std::forward<T>(x));
    }

    template <size_t I, typename T>
    std::string operator()(T&& x) const {
        return ToStringImpl<std::decay_t<T>>::template apply<I>(std::forward<T>(x));
    }
};

inline constexpr ToStringT const toString{};

template <size_t I, class T>
inline std::string toStrings(T&& x) {
    return toString.operator()<I>(std::forward<T>(x));
}

#endif

/// From string conversion
///
/// \ingroup group-function
/// Converts the `string` to type
#ifdef YENXO_DOXYGEN_INVOKED
template <class T>
inline constexpr auto fromString = [](std::string const&) { return type - dispatched; };
#else
// From string conversion implementation
template <typename T, typename = void>
struct FromStringImpl : FromStringImpl<T, When<true>> {};

// Fall back
template <typename T, bool condition>
struct FromStringImpl<T, When<condition>> {
    static T apply(std::string const&) {
        using Ret = typename T::is_not_convertible_from_string;
        return Ret{};
    }
};

// Types explicitly convertible from string
template <typename T>
struct FromStringImpl<
        T,
        When<detail::Valid<decltype(T(std::declval<std::string>()))>::value>> {
    static T apply(std::string const& x) {
        return T(x);
    }

    static T apply(std::string_view x) {
        return T(x);
    }
};

// Types with specialized EnumTraits
template <class T>
struct FromStringImpl<
        T,
        When<detail::Valid<decltype(EnumTraits<T>::toString(std::declval<T>()))>::value
             && !hasStrings(boost::hana::type_c<EnumTraits<T>>)>> {
    static T apply(std::string_view const& x) {
        for (auto e : EnumTraits<T>::values) {
            if (x == EnumTraits<T>::toString(e)) {
                return e;
            }
        }
        throw StringConversionError(x, boost::hana::type_c<T>);
    }

    static T apply(std::string const& x) {
        apply(std::string_view{x});
    }
};

// Types with specialized EnumTraits
template <class T>
struct FromStringImpl<T, When<detail::Valid<decltype(EnumTraits<T>::strings())>::value>> {
    template <size_t I>
    static typename EnumTraits<T>::Enum applyImpl(boost::hana::size_t<I>,
                                                  std::string_view x) {
        bool found{false};
        boost::hana::for_each(boost::hana::at_c<I>(EnumTraits<T>::strings()),
                              [&](auto s) { found |= s == x; });
        if (found) {
            return EnumTraits<T>::values[I];
        } else {
            return applyImpl(boost::hana::size_c<I + 1>, x);
        }

        return EnumTraits<T>::values[I];
    }

    static typename EnumTraits<T>::Enum applyImpl(
            boost::hana::size_t<EnumTraits<T>::count>,
            std::string_view x) {
        throw StringConversionError(x, boost::hana::type_c<T>);
    }

    static T apply(std::string_view x) {
        return applyImpl(boost::hana::size_c<0>, x);
    }

    static T apply(std::string const& x) {
        return applyImpl(boost::hana::size_c<0>, std::string_view{x});
    }
};

template <typename T>
struct FromStringT {
    auto operator()(std::string const& x) const {
        return FromStringImpl<std::remove_cv_t<std::remove_reference_t<T>>>::apply(x);
    }

    auto operator()(std::string_view x) const {
        return FromStringImpl<std::remove_cv_t<std::remove_reference_t<T>>>::apply(x);
    }

    auto operator()(char const* x) const {
        return (*this)(std::string_view{x});
    }
};

// From string conversion
template <typename T>
inline constexpr FromStringT<T> const fromString;

#endif

} // namespace yenxo
