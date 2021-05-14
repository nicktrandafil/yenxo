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
#include <serialize/type_name.hpp>
#include <serialize/variant_conversion.hpp>

#include <boost/hana.hpp>

#include <iomanip>
#include <ostream>

namespace serialize {

/// \pre `T` should be a Boost.Hana.Struct.
template <class T>
void ostreamImpl(std::ostream& os, T const& x) {
    os << typeName(boost::hana::type_c<T>) << " { ";

    boost::hana::for_each(
            x, boost::hana::fuse([&](auto name, auto value) {
                if constexpr (isOptional(boost::hana::type_c<decltype(value)>)) {
                    if (value.has_value()) {
                        os << boost::hana::to<char const*>(name) << ": " << *value
                           << "; ";
                    }
                } else if constexpr (isContainer(boost::hana::type_c<decltype(value)>)) {
                    os << boost::hana::to<char const*>(name);
                    if constexpr (isPair(boost::hana::type_c<typename decltype(
                                                 value)::value_type>)) {
                        os << ": { ";
                        for (auto const& x : value) {
                            os << x.first << ": " << x.second << "; ";
                        }
                        os << "}; ";
                    } else {
                        auto const s = value.size();
                        std::size_t i = 0;
                        os << ": [";
                        for (auto const& x : value) {
                            os << x << ((i++ == s - 1) ? "" : ", ");
                        }
                        os << "]; ";
                    }
#if SERIALIZE_ENABLE_TYPE_SAFE
                } else if constexpr (strongTypeDef(boost::hana::type_c<T>)) {
                    os << static_cast<type_safe::underlying_type<T>>(x);
#endif
                } else {
                    os << boost::hana::to<char const*>(name) << ": " << value << "; ";
                }
            }));

    os << "}";
}

namespace trait {

/// Enables `std::ostream& operator<<(std::ostream&, Derived)` for `Derived`
/// \ingroup group-traits-opt-in
/// \pre `T` should be a Boost.Hana.Struct.
template <typename Derived>
struct OStream {
    friend std::ostream& operator<<(std::ostream& os, Derived const& x) {
        ostreamImpl(os, x);
        return os;
    }
};

} // namespace trait
} // namespace serialize

/// Enables `std::ostream& operator<<(std::ostream&, T)` for `T`
/// \ingroup group-traits-opt-in
/// \pre `T` should be a Boost.Hana.Struct.
/// \see serialize::trait::OStream.
#define SERIALIZE_OSTREAM_OPERATOR(T)                                                    \
    friend std::ostream& operator<<(std::ostream& os, T const& x) {                      \
        serialize::ostreamImpl(os, x);                                                   \
        return os;                                                                       \
    }

/// Enables `std::ostream& operator<<(std::ostream&, T)` for `T`
/// \ingroup group-traits-opt-in
/// \pre `T` should be `toVariantConvertible()`.
///
/// The ostream operator dumps the JSON of the value.
#define SERIALIZE_JSON_OSTREAM_OPERATOR(T)                                               \
    friend std::ostream& operator<<(std::ostream& os, T const& x) {                      \
        return os << serialize::toVariant(x).toPrettyJson();                             \
    }
