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

#include <serialize/meta.hpp>
#include <serialize/when.hpp>

#include <array>

namespace serialize {

/// Enum traits
/// \ingroup group-enum
///
/// To describe an enum's properties specialize the template for the enum,
/// and define the properties:
/// * Enum typedef;
/// * `static constexpr size_t count` member;
/// * `static constexpr std::array<Enum, count> values` member collection;
/// * `static char const* toString(Enum)` member function;
/// * `static constexpr auto strings()` member function that returns `boost::hana::tuple` of `boost::hana::tuple` of `char const*`s (optional).
///
/// The `EnumTraits` is used by `toString`, `fromString`, `toVariant` and `fromVariant`
/// function objects to do the job.
///
/// Define enum,
/// @snippet test/string_conversion.cpp enum1
///
/// specialize the EnumTraits,
/// @snippet test/string_conversion.cpp enum1_traits
///
/// use `serialize::toString` and `fromString`.
/// @snippet test/string_conversion.cpp enum1_string
///
/// Another way to describe properties of an enum (say `E`) is to define a struct (say `ETraits`)
/// with the properties mentioned above and declare a function in the same namespace
/// as the enum `ETraits traits(E)`.
///
/// Define enum,
/// @snippet test/string_conversion.cpp enum2
///
/// define trait struct,
/// @snippet test/string_conversion.cpp enum2_traits
///
/// provide `traits` function definition,
/// @snippet test/string_conversion.cpp enum2_traits_adl
///
/// use `toString` and `fromString`.
/// @snippet test/string_conversion.cpp enum2_string
#ifdef SERIALIZE_DOXYGEN_INVOKED
template <class Enum>
struct EnumTraits;
#else
template <class E, class = When<true>>
struct EnumTraits;
#endif

template <class T>
struct EnumTraits<T,
                  When<std::is_enum_v<T> &&
                       detail::Valid<decltype(traits(std::declval<T>()))>::value>>
    : decltype(traits(std::declval<T>())) {};

} // namespace serialize
