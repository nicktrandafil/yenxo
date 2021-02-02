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

#include <serialize/variant_traits.hpp>

#include <catch2/catch.hpp>

namespace {

struct PersonOpt {
    std::optional<std::string> x;
    std::optional<int> y;
};

struct Person {
    SERIALIZE_TO_VARIANT(Person)
    SERIALIZE_FROM_VARIANT(Person)
    SERIALIZE_UPDATE_FROM_VARIANT(Person)
    SERIALIZE_UPDATE_FROM_OPT(Person, PersonOpt)
    std::string x;
    int y;
};

struct Person2 {
    SERIALIZE_TO_VARIANT_P(Person2, serialize::trait::VarPolicy)
    SERIALIZE_FROM_VARIANT_P(Person2, serialize::trait::VarPolicy)
    SERIALIZE_UPDATE_FROM_VARIANT_P(Person2, serialize::trait::VarPolicy)
    std::string x;
    int y;
};

} // namespace

TEST_CASE("Check SERIALIZE_TO_VARIANT", "[variant_traits]") {
    REQUIRE(serialize::toVariant(Person{"1", 1})
            == serialize::VariantMap{{"x", "1"}, {"y", 1}});
}

TEST_CASE("Check SERIALIZE_TO_VARIANT_P", "[variant_traits]") {
    REQUIRE(serialize::toVariant(Person2{"1", 1})
            == serialize::VariantMap{{"x", "1"}, {"y", 1}});
}

TEST_CASE("Check SERIALIZE_FROM_VARIANT", "[variant_traits]") {
    auto const actual =
            serialize::fromVariant<Person>(serialize::VariantMap{{"x", "1"}, {"y", 1}});
    REQUIRE(actual.x == "1");
    REQUIRE(actual.y == 1);
}

TEST_CASE("Check SERIALIZE_FROM_VARIANT_P", "[variant_traits]") {
    auto const actual =
            serialize::fromVariant<Person2>(serialize::VariantMap{{"x", "1"}, {"y", 1}});
    REQUIRE(actual.x == "1");
    REQUIRE(actual.y == 1);
}

TEST_CASE("Check SERIALIZE_UPDATE_FROM_VARIANT", "[variant_traits]") {
    Person p{"1", 1};
    p.updateVar(serialize::VariantMap{{"x", "2"}, {"y", 2}});
    REQUIRE(p.x == "2");
    REQUIRE(p.y == 2);
}

TEST_CASE("Check SERIALIZE_UPDATE_FROM_VARIANT_P", "[variant_traits]") {
    Person2 p{"1", 1};
    p.updateVar(serialize::VariantMap{{"x", "2"}, {"y", 2}});
    REQUIRE(p.x == "2");
    REQUIRE(p.y == 2);
}

TEST_CASE("Check SERIALIZE_UPDATE_FROM_OPT", "[variant_traits]") {
    Person p{"1", 1};
    p.updateOpt(PersonOpt{"2", 2});
    REQUIRE(p.x == "2");
    REQUIRE(p.y == 2);
}

BOOST_HANA_ADAPT_STRUCT(Person, x, y);
BOOST_HANA_ADAPT_STRUCT(PersonOpt, x, y);
BOOST_HANA_ADAPT_STRUCT(Person2, x, y);
