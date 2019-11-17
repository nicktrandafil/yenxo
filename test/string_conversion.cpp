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

#include <serialize/string_conversion.hpp>

#include <catch2/catch.hpp>

using namespace serialize;

namespace {

struct Hobby {
    explicit Hobby(std::string const& str) : str(str) {}
    [[maybe_unused]] explicit operator std::string() const { return str; }
    bool operator==(Hobby const& rhs) const { return str == rhs.str; }
    std::string str;
};

/// [enum2]
enum class E {
    e1,
    e2
};
/// [enum2]

/// [enum2_traits]
struct ETraits {
    using Enum [[maybe_unused]] = E;
    static constexpr size_t count = 2;
    static constexpr std::array<Enum, count> values{Enum::e1, Enum::e2};
    static char const* toString(Enum x) {
        switch (x) {
        case Enum::e1: return "e1";
        case Enum::e2: return "e2";
        }
        throw 1;
    }
};
/// [enum2_traits]

/// [enum2_traits_adl]
ETraits traits(E) { return {}; }
/// [enum2_traits_adl]

// [enum1]
enum class E2 {
    v3,
    v4
};
// [enum1]

} // namespace

/// [enum1_traits]
namespace serialize {
template <>
struct EnumTraits<E2> {
    using Enum = E2;
    static constexpr size_t count = 2;
    static constexpr std::array<Enum, count> values = {Enum::v3, Enum::v4};
    static char const* toString(Enum e) {
        switch (e) {
        case Enum::v3: return "v3";
        case Enum::v4: return "v4";
        }
        throw 0;
    }
    static constexpr auto strings() {
        return boost::hana::make_tuple(
            boost::hana::make_tuple("v3", "3"),
            boost::hana::make_tuple("v4", "4"));
    }
};
} // namespace serialize
/// [enum1_traits]

TEST_CASE("Check toString/fromString", "[string_conversion]") {
    Hobby const hobby("abc");
    std::string const str("abc");

    // explicit conversion to string
    REQUIRE(str == toString(hobby));
    REQUIRE(hobby == fromString<Hobby>(str));

    /// [enum2_string]
    REQUIRE(toString(E::e1) == "e1");
    REQUIRE(fromString<E>("e2") == E::e2);
    REQUIRE_THROWS_AS(toString(E(9)), int);
    /// [enum2_string]

    /// [enum1_string]
    REQUIRE(toString(E2::v3) == "v3");
    REQUIRE(fromString<E2>("v4") == E2::v4);
    REQUIRE(fromString<E2>("4") == E2::v4);
    /// [enum1_string]

    // std::to_string
    REQUIRE(toString(int(1)) == "1");
}
