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
#include <serialize/string_conversion.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace serialize;


namespace {


struct Hobby {
    explicit Hobby(std::string const& str) : str(str) {}
    [[maybe_unused]] explicit operator std::string() const { return str; }
    bool operator==(Hobby const& rhs) const { return str == rhs.str; }
    std::string str;
};


enum class E {
    e1,
    e2
};


struct ETraits {
    using Enum [[maybe_unused]] = E;
    [[maybe_unused]] static constexpr size_t count = 2;
    [[maybe_unused]] static constexpr std::array<Enum, count> values{Enum::e1, Enum::e2};
    [[maybe_unused]] static char const* toString(Enum x) {
        switch (x) {
        case Enum::e1: return "e1";
        case Enum::e2: return "e2";
        }
        throw 1;
    }
};


[[maybe_unused]] ETraits traits(E) { return {}; }


}


TEST_CASE("Check toString/fromString", "[string_conversion]") {
    Hobby const hobby("abc");
    std::string const str("abc");

    // explicit conversion to string
    REQUIRE(str == toString(hobby));
    REQUIRE(hobby == fromString<Hobby>(str));

    // EnumTraits
    REQUIRE(toString(E::e1) == "e1");
    REQUIRE(fromString<E>("e2") == E::e2);
    REQUIRE_THROWS_AS(toString(E(9)), int);

    // std::to_string
    REQUIRE(toString(int(1)) == "1");
}


TEST_CASE("Check iparseWithSuffix", "[string_conversion]") {
    struct X {};
    REQUIRE(iparseWithSuffix<X>("1.5 suf", "suf") == 1.5);
    REQUIRE(iparseWithSuffix<X>("1.5suf", "suf") == 1.5);
    REQUIRE_THROWS_AS(iparseWithSuffix<X>("1.5 su", "suf"), StringConversionError);
    REQUIRE_THROWS_AS(iparseWithSuffix<X>("suf", "suf"), StringConversionError);
    REQUIRE_THROWS_AS(iparseWithSuffix<X>("1.5", "suf"), StringConversionError);
    REQUIRE_THROWS_AS(iparseWithSuffix<X>("1.5 l suf", "suf"), StringConversionError);
    REQUIRE_THROWS_WITH(iparseWithSuffix<X>("1.5 su", "suf"), "'1.5 su' is not a 'X' value");
}
