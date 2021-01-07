/*
  MIT License

  Copyright (c) 2019 Nicolai Trandafil

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

#include <boost/hana/concat.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/prepend.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>

#include <type_traits>
#include <utility>

namespace serialize {

/// \defgroup group-directives Directives
/// \ingroup group-struct
/// Directives to change default serialization and deserialization behavior.
/// The directives can have any order.

/// A default value tag
/// \ingroup group-directives
///
/// This directive can be used to provide a default value for a struct member.
/// Providing a default value the member implicitly becomes optional.
///
/// \code
/// DEFINE_STRUCT(MyStruct
///     (string, name, Default("Siri"))
/// );
/// \endcode
template <class T>
struct Default {
    template <class U, class = std::enable_if_t<
                           !std::is_trivial_v<T> &&
                           !std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>, Default>>>
    explicit Default(U&& x) : value(std::forward<U>(x)) {}

    template <class U, class = std::enable_if_t<
                           std::is_trivial_v<T> &&
                           !std::is_same_v<U, Default>>>
    constexpr explicit Default(U x) : value(x) {}

    T value;
};

template <class T>
Default(T)->Default<T>;

/// A name tag
/// \ingroup group-directives
///
/// This directive can be used to provide different name for a struct member.
///
/// \code
/// DEFINE_STRUCT(MyStruct
///     (string, name, Name("surname"))
/// );
/// \endcode
struct Name {
    constexpr Name(char const* x) : value(x) {}
    char const* value;
};

template <class T>
struct WrappedInDefault : std::false_type {};

template <class T>
struct WrappedInDefault<Default<T>> : std::true_type {};

constexpr auto const unwrap = [](auto const& x) {
    return boost::hana::make_pair(boost::hana::at_c<0>(x), boost::hana::at_c<1>(x).value);
};

template <class M>
auto filterDefaults(M&& m) {
    return boost::hana::unpack(
        boost::hana::transform(
            boost::hana::filter(
                boost::hana::transform(
                    std::forward<M>(m),
                    [](auto const& x) {
                        return boost::hana::prepend(
                            boost::hana::remove_if(
                                x,
                                [](auto const& x) {
                                    return std::negation<WrappedInDefault<std::remove_cv_t<std::remove_reference_t<decltype(x)>>>>();
                                }),
                            boost::hana::at_c<0>(x));
                    }),
                [](auto const& x) {
                    return boost::hana::length(x) > boost::hana::size_c<1>;
                }),
            unwrap),
        boost::hana::make_map);
}

template <class M>
auto filterNames(M&& m) {
    return boost::hana::unpack(
        boost::hana::transform(
            boost::hana::filter(
                boost::hana::transform(
                    std::forward<M>(m),
                    [](auto const& x) {
                        return boost::hana::prepend(
                            boost::hana::remove_if(
                                x,
                                [](auto const& x) {
                                    return std::negation<std::is_same<Name, std::remove_cv_t<std::remove_reference_t<decltype(x)>>>>();
                                }),
                            boost::hana::at_c<0>(x));
                    }),
                [](auto const& x) {
                    return boost::hana::length(x) > boost::hana::size_c<1>;
                }),
            unwrap),
        boost::hana::make_map);
}

} // namespace serialize
