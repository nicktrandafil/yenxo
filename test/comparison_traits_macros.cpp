/*
  MIT License

  Copyright (c) 2021 Nicolai Trandafil

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

#include <serialize/comparison_traits.hpp>

#include <catch2/catch.hpp>

namespace {

struct Person {
    SERIALIZE_EQUALITY_COMPARISON_OPERATORS(Person)
    std::string x;
    int y;
};

} // namespace

TEST_CASE("Check SERIALIZE_EQUALITY_COMPARISON_OPERATORS", "[comparison_traits]") {
    Person const x{"1", 1};
    Person const y{"1", 1};
    Person const z{"1", 2};
    REQUIRE(x == y);
    REQUIRE(x != z);
}

BOOST_HANA_ADAPT_STRUCT(Person, x, y);
