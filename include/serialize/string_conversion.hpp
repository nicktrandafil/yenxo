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

#include <serialize/enum_traits.hpp>
#include <serialize/exception.hpp>
#include <serialize/meta.hpp>
#include <serialize/type_name.hpp>

#include <boost/hana/for_each.hpp>

#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace serialize {

/// To string conversion
///
/// \ingroup group-function
/// Converts the `type` to string
#ifdef SERIALIZE_DOXYGEN_INVOKED
constexpr auto toString = [](auto&& type) {
    return type - dispatched;
};
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
struct ToStringImpl<T,
                    When<detail::Valid<decltype(
                        std::to_string(std::declval<T>()))>::value>> {
    static std::string apply(T const& x) {
        return std::to_string(x);
    }
};

// Types explicitly covertible to string
template <typename T>
struct ToStringImpl<T,
                    When<detail::Valid<decltype(
                        std::string(std::declval<T>()))>::value>> {
    static std::string apply(T const& x) {
        return std::string(x);
    }
};

// Types with specialized EnumTraits
template <typename T>
struct ToStringImpl<T, When<detail::Valid<decltype(
                           EnumTraits<T>::toString(std::declval<T>()))>::value>> {
    static char const* apply(T x) {
        return EnumTraits<T>::toString(x);
    }
};

struct ToStringT {
    template <typename T>
    std::string operator()(T&& x) const {
        return ToStringImpl<std::decay_t<T>>::apply(std::forward<T>(x));
    }
};

constexpr ToStringT const toString{};

#endif

/// From string conversion
///
/// \ingroup group-function
/// Converts the `string` to type
#ifdef SERIALIZE_DOXYGEN_INVOKED
template <class T>
constexpr auto fromString = [](std::string const&) {
    return type - dispatched;
};
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
struct FromStringImpl<T,
                      When<detail::Valid<decltype(
                          T(std::declval<std::string>()))>::value>> {
    static T apply(std::string const& x) {
        return T(x);
    }
};

// Types with specialized EnumTraits
template <class T>
struct FromStringImpl<T, When<
                             detail::Valid<decltype(
                                 EnumTraits<T>::toString(std::declval<T>()))>::value &&
                             !hasStrings(boost::hana::type_c<EnumTraits<T>>)>> {
    static T apply(std::string const& x) {
        for (auto e : EnumTraits<T>::values) {
            if (x == EnumTraits<T>::toString(e)) { return e; }
        }
        throw StringConversionError(x, boost::hana::type_c<T>);
    }
};

// Types with specialized EnumTraits
template <class T>
struct FromStringImpl<T, When<
                             detail::Valid<decltype(EnumTraits<T>::strings())>::value>> {
    template <size_t I>
    static typename EnumTraits<T>::Enum applyImpl(
        boost::hana::size_t<I>, std::string const& x) {
        bool found{false};
        boost::hana::for_each(boost::hana::at_c<I>(EnumTraits<T>::strings()),
                              [&](auto s) {
                                  found |= strcmp(s, x.c_str()) == 0;
                              });
        if (found) {
            return EnumTraits<T>::values[I];
        } else {
            return applyImpl(boost::hana::size_c<I + 1>, x);
        }

        return EnumTraits<T>::values[I];
    }

    static typename EnumTraits<T>::Enum applyImpl(
        boost::hana::size_t<EnumTraits<T>::count>, std::string const& x) {
        throw StringConversionError(x, boost::hana::type_c<T>);
    }

    static T apply(std::string const& x) {
        return applyImpl(boost::hana::size_c<0>, x);
    }
};

template <typename T>
struct FromStringT {
    auto operator()(std::string const& x) const {
        return FromStringImpl<std::remove_cv_t<std::remove_reference_t<T>>>::apply(x);
    }
};

// From string conversion
template <typename T>
constexpr FromStringT<T> const fromString;

#endif

} // namespace serialize
