/*
  MIT License

  Copyright (c) 2019 Nicolai Trandafil

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

#include <serialize/variant_conversion.hpp>

#include <catch2/catch.hpp>

#include <map>
#include <set>

using namespace serialize;
using namespace boost::hana::literals;

namespace {

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
    static constexpr std::string_view typeName() noexcept { return "E"; }
};

[[maybe_unused]] ETraits traits(E) { return {}; }

struct Test {
    static Variant toVariant(Test) { return {}; }
    static Test fromVariant(Variant const&) { return {}; }
    bool operator==(Test const&) const { return true; }
};

enum E2 {
    val1,
    val2
};

struct E2Traits {
    using Enum = E2;
    static constexpr auto count = 2;
    [[maybe_unused]] static constexpr std::array<Enum, count> values{Enum::val1, Enum::val2};
    static char const* toString(Enum x) {
        switch (x) {
        case Enum::val1: return "val1";
        case Enum::val2: return "val2";
        }
        throw 1;
    }
};

[[maybe_unused]] E2Traits traits(E2) { return {}; }

} // namespace

TEST_CASE("Check toVariant/fromVariant", "[variant_conversion]") {
    REQUIRE(toVariant(Variant(1)) == Variant(1));
    struct X {
        static serialize::Variant toVariant(X const&) { return Variant(1); }
    };
    REQUIRE(toVariant(X()) == Variant(1));
    boost::hana::for_each(Variant::Types::rebind_t<boost::hana::tuple>(), [](auto type) {
        REQUIRE(toVariant(typename decltype(type)::type()) ==
                Variant(typename decltype(type)::type()));
    });
    REQUIRE(toVariant(std::map<std::string, int>{{"1", 1}}) ==
            Variant(VariantMap{{"1", Variant(1)}}));
    REQUIRE(toVariant(std::pair(1, "1")) ==
            Variant(VariantMap{{"first", Variant(1)}, {"second", Variant("1")}}));

    REQUIRE(toVariant(E::e1) == Variant("e1"));
    REQUIRE(fromVariant<E>(Variant("e2")) == E::e2);

    using V = std::variant<int, std::string>;
    REQUIRE(toVariant(V("a")) == Variant("a"));
    REQUIRE(toVariant(V(1)) == Variant(1));

    REQUIRE(fromVariant<V>(Variant(1)) == V(1));
    REQUIRE(fromVariant<V>(Variant("a")) == V("a"));
    REQUIRE_THROWS_AS(fromVariant<V>(Variant(1.2)) == V("a"), VariantBadType);
    REQUIRE_THROWS_WITH(fromVariant<V>(Variant(1.5)) == V("a"), "'1.5' is not of type 'one of [int32, string]'");

    REQUIRE(fromVariant<Test>(Variant()) == Test());
    REQUIRE(toVariant(Test()) == Variant());

    REQUIRE(Variant(1) == toVariant(Variant(1)));

    REQUIRE(Variant("val1") == toVariant(E2::val1));

    SECTION("vector") {
        REQUIRE_THROWS_WITH(fromVariant<std::vector<E>>(Variant(VariantVec{Variant("e1"), Variant("e3")})), ".1: 'e3' is not of type 'E'");
    }

    SECTION("set") {
        REQUIRE_THROWS_WITH(fromVariant<std::set<E>>(Variant(VariantVec{Variant("e1"), Variant("e3")})), ".1: 'e3' is not of type 'E'");
    }

    SECTION("hana map") {
        auto tmp = boost::hana::make_map(
            boost::hana::make_pair(BOOST_HANA_STRING("a"), E()),
            boost::hana::make_pair(BOOST_HANA_STRING("b"), E()));

        REQUIRE_THROWS_WITH(fromVariant2(tmp, Variant(VariantMap{{"a", Variant("e1")}, {"b", Variant("e3")}})), ".b: 'e3' is not of type 'E'");
    }

    SECTION("hana string") {
        using Str = decltype("str"_s);
        REQUIRE_NOTHROW(fromVariant<Str>(Variant("str")));
        REQUIRE_THROWS_WITH(fromVariant<Str>(Variant("strr")),
                            "'strr' is not of type 'str literal'");
        REQUIRE(toVariant("str"_s) == Variant("str"));
    }
}
