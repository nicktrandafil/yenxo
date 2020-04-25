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
        : std::runtime_error(error), input(input), expected(expected),
          error_pos(error_pos) {
    }

    /// Test if the error is parse error
    ///
    /// There can be three types of errors:
    /// 1. syntax errors (parse errors);
    /// 2. constraint errors (object_property_count_limit, object_depth_limit,
    /// array_length_limit);
    /// 3. range errors (too big integers).
    ///
    /// Parse errors can be obtained via `prettyParseerror` (in addition to `what`).
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
///
/// Supports deep objects and arrays. The function, for overhead reasons,
/// limits the resulting object dimensions:
/// * object_property_count_limit=20;
/// * object_depth_limit=20;
/// * array_length_limit=20.
///
/// Examples
/// --------
/// Scalar parameter:
/// @snippet test/query_string.cpp simple_value
/// The `_j` string literal produces `Variant` from a JSON. The literal is introduced just
/// to shorten examples and is not available in the library interface.
///
/// Array parameter by duplicating the parameter's name:
/// @snippet test/query_string.cpp array_by_duplicate
///
/// Array parameter by using empty square brackets:
/// @snippet test/query_string.cpp array_by_empty_brackets
///
/// For Array parameters indexes can be specified (0 - first index):
/// @snippet test/query_string.cpp array_by_index
/// The array have size of max index plus one.
/// The array have `null`-values for unspecified indexes.
///
/// Object parameter:
/// @snippet test/query_string.cpp simple_object
///
/// Brackets can be (and recommended to be) percent-encoded:
/// @snippet test/query_string.cpp simple_object_enc
///
/// Array indexes (`[2]`) and object keys (`[b]`) can be composed in any way:
/// @snippet test/query_string.cpp array_in_object
/// @snippet test/query_string.cpp object_in_array
/// Empty brackets can occur only on a key ending.
///
/// Error message may refer to <a href="https://www.ietf.org/rfc/rfc3986.txt">ABNF
/// grammer</a>
///
/// \ingroup group-http
/// \throw QueryStringError
/// \return VariantMap
Variant query_string(std::string const& str);

} // namespace serialize
