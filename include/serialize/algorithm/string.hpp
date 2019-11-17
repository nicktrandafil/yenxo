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

#include <serialize/string_conversion.hpp>

#include <algorithm>
#include <string>

namespace serialize {

/// \ingroup group-utility
/// Test case insensitively if a string ends with suffix
inline bool iendsWith(std::string const& str, std::string const& end) {
    return std::equal(
        end.rbegin(), end.rend(), str.rbegin(),
        [](auto c1, auto c2) {
            return std::tolower(c1) == std::tolower(c2);
        });
}

/// Parse a double with siffux
/// \ingroup group-utility
/// \throws StringConversionError
template <typename T>
double iparseWithSuffix(std::string const& str, std::string const& suffix) {
    if (!iendsWith(str, suffix)) {
        throw StringConversionError(str, boost::hana::type_c<T>);
    }

    char* end;
    auto const ret = std::strtod(str.c_str(), &end);

    if (end == str.c_str()) {
        throw StringConversionError(str, boost::hana::type_c<T>);
    }
    if (!std::all_of(static_cast<char const*>(end),
                     str.c_str() + str.size() - suffix.size(),
                     [](auto x) { return std::isspace(x); })) {
        throw StringConversionError(str, boost::hana::type_c<T>);
    }

    return ret;
}

} // namespace serialize
