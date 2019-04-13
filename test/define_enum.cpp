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
    (e3,),
    (e4, 10),
    (e5, 11, "e55"),
    (e6,, "e66"),
    (e7,,),
    e8,
    e9,
    e10
);


}


TEST_CASE("Check DEFINE_ENUM", "[define_enum]") {
    REQUIRE(toString(E::e1) == "e1");
    REQUIRE(fromString<E>("e1") == E::e1);

    REQUIRE(toString(E::e2) == "e2");
    REQUIRE(fromString<E>("e2") == E::e2);

    REQUIRE(toString(E::e3) == "e3");
    REQUIRE(fromString<E>("e3") == E::e3);

    REQUIRE(toString(E::e4) == "e4");
    REQUIRE(fromString<E>("e4") == E::e4);
    REQUIRE(static_cast<std::underlying_type_t<E>>(E::e4) == 10);

    REQUIRE(toString(E::e5) == "e55");
    REQUIRE(fromString<E>("e55") == E::e5);

    REQUIRE(toString(E::e6) == "e66");
    REQUIRE(fromString<E>("e66") == E::e6);

    REQUIRE(toString(E::e7) == "e7");
    REQUIRE(fromString<E>("e7") == E::e7);

    REQUIRE(toString(E::e8) == "e8");
    REQUIRE(fromString<E>("e8") == E::e8);

    REQUIRE(toString(E::e9) == "e9");
    REQUIRE(fromString<E>("e9") == E::e9);

    REQUIRE(toString(E::e10) == "e10");
    REQUIRE(fromString<E>("e10") == E::e10);
}
