/*
  MIT License

  Copyright (c) 2020 Nicolai Trandafil

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

#include <serialize/variant_fwd.hpp>

namespace serialize {

/// Query string parse error
/// \ingroup group-exceptions
class QueryStringError : public std::runtime_error {
public:
    explicit QueryStringError(std::string const& error, std::string const& input = {},
                              std::string const& expected = {},
                              std::size_t error_pos = std::string::npos)
        : std::runtime_error(error), input(input), expected(expected), error_pos(error_pos) {
    }

    bool isParseError() const noexcept {
        return error_pos != std::string::npos;
    }

    std::string prettyParseError() const;

private:
    std::string input;
    std::string expected;
    std::size_t error_pos;
};

/// Parse a query string
/// Supports deep objects and arrays.
/// \ingroup group-http
/// \throw QueryStringError
/// \return VariantMap
Variant query_string(std::string const& str);

} // namespace serialize
