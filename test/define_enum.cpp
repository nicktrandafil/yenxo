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


// tested
#include <serialize/define_enum.hpp>
#include <serialize/string_conversion.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace serialize;


namespace {


DEFINE_ENUM(E,
    e1,
    (e2),
    (e3,)
);


}


TEST_CASE("Check DEFINE_ENUM", "[define_enum]") {
    REQUIRE(toString(E::e1) == "e1");
    REQUIRE(fromString<E>("e1") == E::e1);

    REQUIRE(toString(E::e2) == "e2");
    REQUIRE(fromString<E>("e2") == E::e2);

    REQUIRE(toString(E::e3) == "e3");
    REQUIRE(fromString<E>("e3") == E::e3);
}
