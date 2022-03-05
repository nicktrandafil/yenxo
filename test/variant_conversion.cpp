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

#include "matchers.hpp"

#include <yenxo/define_struct.hpp>
#include <yenxo/variant_conversion.hpp>
#include <yenxo/variant_traits.hpp>

#include <catch2/catch.hpp>

#include <boost/hana/fuse.hpp>

#include <map>
#include <set>

using namespace yenxo;
using namespace boost::hana::literals;

namespace {

enum class E { e1, e2 };

struct ETraits {
    using Enum [[maybe_unused]] = E;
    [[maybe_unused]] static constexpr size_t count = 2;
    [[maybe_unused]] static constexpr std::array<Enum, count> values{Enum::e1, Enum::e2};
    [[maybe_unused]] static char const* toString(Enum x) {
        switch (x) {
        case Enum::e1:
            return "e1";
        case Enum::e2:
            return "e2";
        }
        throw 1;
    }
    static constexpr std::string_view typeName() noexcept {
        return "E";
    }
};

[[maybe_unused]] ETraits traits(E) {
    return {};
}

struct Test {
    static Variant toVariant(Test) {
        return {};
    }
    static Test fromVariant(Variant const&) {
        return {};
    }
    bool operator==(Test const&) const {
        return true;
    }
};

enum E2 { val1, val2 };

struct E2Traits {
    using Enum = E2;
    static constexpr auto count = 2;
    [[maybe_unused]] static constexpr std::array<Enum, count> values{Enum::val1,
                                                                     Enum::val2};
    static char const* toString(Enum x) {
        switch (x) {
        case Enum::val1:
            return "val1";
        case Enum::val2:
            return "val2";
        }
        throw 1;
    }
};

[[maybe_unused]] E2Traits traits(E2) {
    return {};
}

} // namespace

TEST_CASE("Check toVariant/fromVariant", "[variant_conversion]") {
    SECTION("`toVariant`/`fromVariant` static member functions") {
        REQUIRE(fromVariant<Test>(Variant()) == Test());
        REQUIRE(toVariant(Test()) == Variant());
    }

    SECTION("Variant") {
        REQUIRE(Variant(1) == toVariant(Variant(1)));
    }

    SECTION("enum class") {
        REQUIRE(toVariant(E::e1) == Variant("e1"));
        REQUIRE(fromVariant<E>(Variant("e2")) == E::e2);
    }

    SECTION("enum") {
        REQUIRE(Variant("val1") == toVariant(E2::val1));
    }

    SECTION("std::map") {
        REQUIRE(toVariant(std::map<std::string, int>{{"1", 1}})
                == Variant(VariantMap{{"1", Variant(1)}}));
    }

    SECTION("std::pair") {
        REQUIRE(toVariant(std::pair(1, "1"))
                == Variant(VariantMap{{"first", Variant(1)}, {"second", Variant("1")}}));
    }

    SECTION("std::variant") {
        using V = std::variant<int, std::string>;
        REQUIRE(toVariant(V("a")) == Variant("a"));
        REQUIRE(toVariant(V(1)) == Variant(1));

        REQUIRE(fromVariant<V>(Variant(1)) == V(1));
        REQUIRE(fromVariant<V>(Variant("a")) == V("a"));
        REQUIRE_THROWS_AS(fromVariant<V>(Variant(1.2)) == V("a"), VariantBadType);
        REQUIRE_THROWS_WITH(fromVariant<V>(Variant(1.5)) == V("a"),
                            "'1.5' is not of type 'one of [int32, string]'");
    }

    SECTION("std::vector") {
        REQUIRE_THROWS_MATCHES(
                fromVariant<std::vector<E>>(
                        Variant(VariantVec{Variant("e1"), Variant("e3")})),
                VariantBadType,
                ExceptionIs<VariantBadType>("'e3' is not of type 'E'", "/1"));
    }

    SECTION("std::set") {
        REQUIRE_THROWS_WITH(fromVariant<std::set<E>>(
                                    Variant(VariantVec{Variant("e1"), Variant("e3")})),
                            "'e3' is not of type 'E'");
    }

    SECTION("std::array") {
        std::array<int, 2> const a{1, 2};

        SECTION("ok") {
            Variant const v(VariantVec{Variant{1}, Variant{2}});
            REQUIRE(v == toVariant(a));
            REQUIRE(fromVariant<std::array<int, 2>>(v) == a);
        }

        SECTION("size does not match") {
            Variant const v(VariantVec{Variant{1}});
            REQUIRE_THROWS_WITH((fromVariant<std::array<int, 2>>(v)),
                                "expected size of the list is 2, actual 1");
        }

        SECTION("element bad type") {
            Variant const v(VariantVec{Variant{"1"}, Variant(1)});
            REQUIRE_THROWS_WITH((fromVariant<std::array<int, 2>>(v)),
                                "expected 'int32', actual 'string'");
        }
    }

    SECTION("hana::map") {
        auto tmp = boost::hana::make_map(
                boost::hana::make_pair(BOOST_HANA_STRING("a"), E()),
                boost::hana::make_pair(BOOST_HANA_STRING("b"), E()));

        REQUIRE_THROWS_WITH(fromVariant2(tmp,
                                         Variant(VariantMap{{"a", Variant("e1")},
                                                            {"b", Variant("e3")}})),
                            "'e3' is not of type 'E'");
    }

    SECTION("hana::string") {
        using Str = decltype("str"_s);
        REQUIRE_NOTHROW(fromVariant<Str>(Variant("str")));
        REQUIRE_THROWS_WITH(fromVariant<Str>(Variant("strr")),
                            "'strr' is not of type 'str literal'");
        REQUIRE(toVariant("str"_s) == Variant("str"));
    }

    SECTION("hana::tuple") {
        using namespace std::string_literals;
        auto tuple = boost::hana::make_tuple(1, "2"s);

        SECTION("ok") {
            Variant const v(VariantVec{Variant{1}, Variant{"2"}});
            REQUIRE(v == toVariant(tuple));
            REQUIRE(boost::hana::equal(tuple, fromVariant<decltype(tuple)>(v)));
        }

        SECTION("size does not match") {
            Variant const v(VariantVec{Variant{1}});
            REQUIRE_THROWS_WITH((fromVariant<decltype(tuple)>(v)),
                                "expected size of the tuple is 2, actual 1");
        }

        SECTION("element bad type") {
            Variant const v(VariantVec{Variant{"1"}, Variant(2)});
            REQUIRE_THROWS_WITH((fromVariant<decltype(tuple)>(v)),
                                "expected 'int32', actual 'string'");
        }

        REQUIRE(toVariant(tuple) == VariantVec{1, "2"});
    }

    SECTION("hana::Constant") {
        REQUIRE_NOTHROW(yenxo::fromVariant<std::integral_constant<int, 2>>(2));
        REQUIRE_THROWS(yenxo::fromVariant<std::integral_constant<int, 2>>(1));
    }

    SECTION("Variant supported types") {
        SECTION("auto") {
            boost::hana::for_each(
                    Variant::Types::rebind_t<boost::hana::tuple>(), [](auto type) {
                        REQUIRE(toVariant(typename decltype(type)::type())
                                == Variant(typename decltype(type)::type()));
                    });
        }

        SECTION("manual") {
            auto const integrals = boost::hana::make_tuple(
                    boost::hana::make_pair(Variant::NullType{}, Variant{}),
                    boost::hana::make_pair(bool{true}, Variant{true}),
                    boost::hana::make_pair(char{0}, Variant{char{0}}),
                    boost::hana::make_pair(uint8_t{0}, Variant{uint8_t{0}}),
                    boost::hana::make_pair(int8_t{0}, Variant{int8_t{0}}),
                    boost::hana::make_pair(uint64_t{0}, Variant{uint64_t{0}}),
                    boost::hana::make_pair(int64_t{0}, Variant{int64_t{0}}),
                    boost::hana::make_pair(uint32_t{0}, Variant{uint32_t{0}}),
                    boost::hana::make_pair(int32_t{0}, Variant{int32_t{0}}),
                    boost::hana::make_pair(uint16_t{0}, Variant{uint16_t{0}}));
            boost::hana::for_each(integrals, boost::hana::fuse([](auto rv, auto vv) {
                                      REQUIRE(toVariant(rv) == vv);
                                      REQUIRE(fromVariant<decltype(rv)>(vv) == rv);
                                  }));
        }
    }
}

namespace {

struct SimpleProperty {
    YENXO_FROM_VARIANT(SimpleProperty)
    BOOST_HANA_DEFINE_STRUCT(SimpleProperty, (int, x));
};

struct SpecialSymbol1 {
    YENXO_FROM_VARIANT(SpecialSymbol1)
    YENXO_DEFINE_STRUCT(SpecialSymbol1, (int, x, Name("x~y~")));
};

struct SpecialSymbol2 {
    YENXO_FROM_VARIANT(SpecialSymbol2)
    YENXO_DEFINE_STRUCT(SpecialSymbol2, (int, x, Name("~x/y/~"))); };  } // namespace

TEST_CASE("Check VariantErr::path()", "[exception]") {
    REQUIRE_THROWS_MATCHES(fromVariant<SimpleProperty>(VariantMap{{"x", "1"}}),
                           VariantBadType,
                           PathIs<VariantBadType>("/x"));
    REQUIRE_THROWS_MATCHES(fromVariant<SpecialSymbol1>(VariantMap{{"x~y~", "1"}}),
                           VariantBadType,
                           PathIs<VariantBadType>("/x~0y~0"));
    REQUIRE_THROWS_MATCHES(fromVariant<SpecialSymbol2>(VariantMap{{"~x/y/~", "1"}}),
                           VariantBadType,
                           PathIs<VariantBadType>("/~0x~1y~1~0"));
}

static_assert(toVariantConvertible(boost::hana::type_c<Variant>));
static_assert(toVariantConvertible(boost::hana::type_c<int>));
static_assert(
        toVariantConvertible(boost::hana::type_c<std::unordered_map<std::string, int>>));
static_assert(toVariantConvertible(boost::hana::type_c<E>));
