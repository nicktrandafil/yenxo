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
#include <variant.hpp>

// 3rd
#include <catch2/catch.hpp>


TEST_CASE("Check Variant", "[Variant]") {
    SECTION("short int") {
        short int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.shortInt());
        REQUIRE_THROWS_AS(Variant().shortInt(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").shortInt(), VariantBadType);
        REQUIRE(Variant().shortIntOr(1) == 1);
    }

    SECTION("integer") {
        int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.integer());
        REQUIRE_THROWS_AS(Variant().integer(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").integer(), VariantBadType);
        REQUIRE(Variant().integerOr(1) == 1);
    }

    SECTION("uint") {
        unsigned int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.uint());
        REQUIRE_THROWS_AS(Variant().uint(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").uint(), VariantBadType);
        REQUIRE(Variant().uintOr(1) == 1);
    }

    SECTION("string") {
        std::string const expected = "ab";
        auto const x = Variant(expected);
        REQUIRE(expected == x.str());
        REQUIRE_THROWS_AS(Variant().str(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant(5).str(), VariantBadType);
        REQUIRE(Variant().strOr("abc") == "abc");
    }

    SECTION("Variant::Vec") {
        auto const expected = Variant::Vec{Variant(1), Variant("ab")};
        auto const x = Variant(expected);
        REQUIRE(expected == x.vec());
        REQUIRE_THROWS_AS(Variant().vec(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant(5).vec(), VariantBadType);
        REQUIRE(Variant().vecOr(Variant::Vec{Variant("abc")}) ==
                Variant::Vec{Variant("abc")});
    }

    SECTION("Variant::Map") {
        auto const expected = Variant::Map{std::pair("1", Variant(1))};
        auto const x = Variant(expected);
        REQUIRE(expected == x.map());
        REQUIRE_THROWS_AS(Variant().map(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant(5).map(), VariantBadType);
        REQUIRE(Variant().mapOr(Variant::Map{std::pair("1", Variant(1))}) ==
                Variant::Map{std::pair("1", Variant(1))});
    }
}


