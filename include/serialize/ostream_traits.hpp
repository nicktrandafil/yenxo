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

#include <boost/hana.hpp>

#include <iomanip>
#include <ostream>

namespace serialize::trait {

/// Enables `std::ostream& operator<<(std::ostream&, Derived)` for `Derived`
/// \ingroup group-traits
template <typename Derived>
struct OStream {
    friend std::ostream& operator<<(std::ostream& os, Derived const& x) {
        os << unqualifiedTypeName<Derived>() << " { ";

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
                                  if constexpr (isOptional(boost::hana::type_c<decltype(value)>)) {
                                      if (value.has_value()) {
                                          os << boost::hana::to<char const*>(name)
                                             << ": "
                                             << *value
                                             << "; ";
                                      }
                                  } else if constexpr (isContainer(boost::hana::type_c<decltype(value)>)) {
                                      os << boost::hana::to<char const*>(name);
                                      if constexpr (isKeyValue(boost::hana::type_c<typename decltype(value)::value_type>)) {
                                          os << ": { ";
                                          for (auto const& x : value) {
                                              os << x.first
                                                 << ": "
                                                 << x.second
                                                 << "; ";
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
                                  } else if constexpr (strongTypeDef(boost::hana::type_c<Derived>)) {
                                      os << static_cast<type_safe::underlying_type<Derived>>(x);
#endif
                                  } else {
                                      os << boost::hana::to<char const*>(name)
                                         << ": "
                                         << value
                                         << "; ";
                                  }
                              }));

        os << "}";

        return os;
    }
};

} // namespace serialize::trait
