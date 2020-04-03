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

#include <serialize/exception.hpp>
#include <serialize/type_name.hpp>
#include <serialize/variant.hpp>

#include <catch2/catch.hpp>

#include <boost/hana.hpp>

#include <rapidjson/document.h>

#include <limits.h>
#include <sstream>

namespace hana = boost::hana;

using namespace serialize;

TEST_CASE("Check Variant", "[Variant]") {
    REQUIRE(Variant().null());
    REQUIRE(!Variant(1).null());

    SECTION("boolean") {
        bool const expected{true};
        auto const x = Variant(expected);
        REQUIRE(expected == x.boolean());
        REQUIRE_THROWS_AS(Variant().boolean(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").boolean(), VariantBadType);
        REQUIRE(Variant().booleanOr(true) == true);
    }

    SECTION("char8") {
        char const expected{-7};
        auto const x = Variant(expected);
        REQUIRE(expected == x.char8());
        REQUIRE_THROWS_AS(Variant().char8(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").char8(), VariantBadType);
        REQUIRE(Variant().char8Or(1) == 1);
    }

    SECTION("uchar8") {
        unsigned char const expected{7};
        auto const x = Variant(expected);
        REQUIRE(expected == x.char8());
        REQUIRE_THROWS_AS(Variant().uchar8(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").uchar8(), VariantBadType);
        REQUIRE(Variant().uchar8Or(1) == 1);
    }

    SECTION("int16") {
        int16_t const expected{6};
        auto const x = Variant(expected);
        REQUIRE(expected == x.int16());
        REQUIRE_THROWS_AS(Variant().int16(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").int16(), VariantBadType);
        REQUIRE(Variant().int16Or(1) == 1);
    }

    SECTION("uint16") {
        uint16_t const expected{3};
        auto const x = Variant(expected);
        REQUIRE(expected == x.uint16());
        REQUIRE_THROWS_AS(Variant().uint16(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").uint16(), VariantBadType);
        REQUIRE(Variant().uint16Or(1) == 1);
    }

    SECTION("int32") {
        int32_t const expected{7};
        auto const x = Variant(expected);
        REQUIRE(expected == x.int32());
        REQUIRE_THROWS_AS(Variant().int32(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").int32(), VariantBadType);
        REQUIRE(Variant().int32Or(1) == 1);
    }

    SECTION("uint32") {
        uint32_t const expected{8};
        auto const x = Variant(expected);
        REQUIRE(expected == x.uint32());
        REQUIRE_THROWS_AS(Variant().uint32(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").uint32(), VariantBadType);
        REQUIRE(Variant().uint32Or(1) == 1);
    }

    SECTION("int64") {
        int64_t const expected{12};
        auto const x = Variant(expected);
        REQUIRE(expected == x.int64());
        REQUIRE_THROWS_AS(Variant().int64(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").int64(), VariantBadType);
        REQUIRE(Variant().int64Or(12) == 12);
    }

    SECTION("uint64") {
        uint64_t const expected{13};
        auto const x = Variant(expected);
        REQUIRE(expected == x.uint64());
        REQUIRE_THROWS_AS(Variant().uint64(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").uint64(), VariantBadType);
        REQUIRE(Variant().uint64Or(expected) == expected);
    }

    SECTION("floating") {
        double const expected{5.1};
        auto const x = Variant(expected);
        REQUIRE(expected == x.floating());
        REQUIRE_THROWS_AS(Variant().floating(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").floating(), VariantBadType);
        REQUIRE(Variant().floatingOr(1.1) == 1.1);
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

    SECTION("empty variant") {
        auto const types = hana::tuple_t<bool, char, unsigned char, int16_t, uint16_t,
                                         int32_t, uint32_t, int64_t, uint64_t, double,
                                         std::string, VariantMap, VariantVec>;

        hana::for_each(types, [&](auto x) {
            REQUIRE_THROWS_AS(static_cast<typename decltype(x)::type>(Variant()),
                              VariantEmpty);
        });
    }

    SECTION("string, Vec, Map is not convertible to anything") {
        auto const types = hana::tuple_t<bool, char, unsigned char, int16_t, uint16_t,
                                         int32_t, uint32_t, int64_t, uint64_t, double>;

        hana::for_each(types, [&](auto x) {
            REQUIRE_THROWS_AS(static_cast<typename decltype(x)::type>(Variant("")),
                              VariantBadType);
            REQUIRE_THROWS_AS(
                    static_cast<typename decltype(x)::type>(Variant(VariantVec())),
                    VariantBadType);
            REQUIRE_THROWS_AS(
                    static_cast<typename decltype(x)::type>(Variant(VariantMap())),
                    VariantBadType);
        });
    }

    SECTION("modifyVec") {
        Variant v(VariantVec{Variant(1)});
        v.modifyVec().push_back(Variant(2));
        Variant expected(VariantVec{Variant(1), Variant(2)});
        REQUIRE(v == expected);

        REQUIRE_THROWS_AS(Variant().modifyVec(), VariantEmpty);
        auto const types = hana::tuple_t<bool, char, unsigned char, int16_t, uint16_t,
                                         int32_t, uint32_t, int64_t, uint64_t, double>;

        hana::for_each(types, [&](auto x) {
            REQUIRE_THROWS_AS(Variant(typename decltype(x)::type()).modifyVec(),
                              VariantBadType);
        });
    }

    SECTION("modifyMap") {
        Variant v(VariantMap{});
        v.modifyMap().emplace("a", Variant(2));
        Variant expected(VariantMap{{"a", Variant(2)}});
        REQUIRE(v == expected);


        REQUIRE_THROWS_AS(Variant().modifyMap(), VariantEmpty);
        auto const types = hana::tuple_t<bool, char, unsigned char, int16_t, uint16_t,
                                         int32_t, uint32_t, int64_t, uint64_t, double>;

        hana::for_each(types, [&](auto x) {
            REQUIRE_THROWS_AS(Variant(typename decltype(x)::type()).modifyMap(),
                              VariantBadType);
        });
    }

    SECTION("allow representable integral type conversions") {
        auto const types = hana::make_tuple(
                hana::type_c<bool>, hana::type_c<char>, hana::type_c<unsigned char>,
                hana::type_c<int16_t>, hana::type_c<uint16_t>, hana::type_c<int32_t>,
                hana::type_c<uint32_t>);

        hana::for_each(types, [&](auto x) {
            hana::for_each(types, [](auto y) {
                using X = typename decltype(x)::type;
                using Y = typename decltype(y)::type;
                using YLimiets = std::numeric_limits<Y>;

                auto const y_min = YLimiets::min();

                // no fails

                // min bound
                auto const y_min_in_x = static_cast<X>(y_min);
                if (std::to_string(y_min) == std::to_string(y_min_in_x)) {
                    REQUIRE(static_cast<Y>(Variant(y_min_in_x)) == y_min);
                    REQUIRE(Variant(y_min_in_x).asOr<Y>(1) == y_min);
                }

                auto const y_max = YLimiets::max();

                // max bound
                auto const y_max_in_x = static_cast<X>(y_max);
                if (std::to_string(y_max) == std::to_string(y_max_in_x)) {
                    REQUIRE(static_cast<Y>(Variant(y_max_in_x)) == y_max);
                    REQUIRE(Variant(y_max_in_x).asOr<Y>(1) == y_max);
                }

                REQUIRE(static_cast<Y>(Variant(X(1))) == Y(1));

                // fails

                // min - 1 out of range
                auto const y_min_in_x_1 = static_cast<X>(static_cast<double>(y_min) - 1);
                if (std::to_string(static_cast<double>(y_min) - 1) ==
                    std::to_string(static_cast<double>(y_min_in_x_1))) {
                    REQUIRE_THROWS_AS(static_cast<Y>(Variant(y_min_in_x_1)),
                                      VariantIntegralOverflow);
                    REQUIRE_THROWS_AS(Variant(y_min_in_x_1).asOr<Y>(1),
                                      VariantIntegralOverflow);
                    REQUIRE_THROWS_WITH(
                            static_cast<Y>(Variant(y_min_in_x_1)),
                            "The type '" + std::string(typeName(boost::hana::type_c<Y>)) +
                                    "' can not hold the value '" +
                                    std::to_string(y_min_in_x_1) + "'");
                    REQUIRE_THROWS_WITH(
                            Variant(y_min_in_x_1).asOr<Y>(1),
                            "The type '" + std::string(typeName(boost::hana::type_c<Y>)) +
                                    "' can not hold the value '" +
                                    std::to_string(y_min_in_x_1) + "'");
                }

                // max + 1 out of range
                auto const y_max_in_x_1 = static_cast<X>(static_cast<double>(y_max) + 1);
                if (std::to_string(static_cast<double>(y_max) + 1) ==
                    std::to_string(static_cast<double>(y_max_in_x_1))) {
                    REQUIRE_THROWS_AS(static_cast<Y>(Variant(y_max_in_x_1)),
                                      VariantIntegralOverflow);
                    REQUIRE_THROWS_AS(Variant(y_max_in_x_1).asOr<Y>(1),
                                      VariantIntegralOverflow);
                    REQUIRE_THROWS_WITH(
                            static_cast<Y>(Variant(y_max_in_x_1)),
                            "The type '" + std::string(typeName(boost::hana::type_c<Y>)) +
                                    "' can not hold the value '" +
                                    std::to_string(y_max_in_x_1) + "'");
                    REQUIRE_THROWS_WITH(
                            Variant(y_max_in_x_1).asOr<Y>(1),
                            "The type '" + std::string(typeName(boost::hana::type_c<Y>)) +
                                    "' can not hold the value '" +
                                    std::to_string(y_max_in_x_1) + "'");
                }
            });
        });

        REQUIRE_THROWS_AS(Variant(1.1).int32(), VariantBadType);
        REQUIRE_THROWS_AS(Variant(Variant::Vec()).int32(), VariantBadType);
        REQUIRE_THROWS_AS(Variant(Variant::Map()).int32(), VariantBadType);
    }

    SECTION("allow integral type to double conversions") {
        auto const types = hana::tuple_t<bool, char, unsigned char, int16_t, uint16_t,
                                         int32_t, uint32_t, int64_t, uint64_t>;

        hana::for_each(types, [&](auto x) {
            auto const lhs = Variant(typename decltype(x)::type()).floating();
            auto const rhs = double(typename decltype(x)::type());
            REQUIRE((!(lhs < rhs) &&
                     !(lhs > rhs)) /* the same lhs == rhs, but without warning */);
        });

        REQUIRE_THROWS_AS(Variant(1.1).int32(), VariantBadType);
        REQUIRE_THROWS_AS(Variant(Variant::Vec()).int32(), VariantBadType);
        REQUIRE_THROWS_AS(Variant(Variant::Map()).int32(), VariantBadType);
    }

    SECTION("compare") {
        using uchar = unsigned char;
        REQUIRE(Variant() != Variant(1));
        REQUIRE(Variant(true) == Variant(true));
        REQUIRE(Variant(char{}) == Variant(char{}));
        REQUIRE(Variant(uchar{}) == Variant(uchar{}));
        REQUIRE(Variant(int16_t{}) == Variant(int16_t{}));
        REQUIRE(Variant(uint16_t{}) == Variant(uint16_t{}));
        REQUIRE(Variant(int32_t{}) == Variant(int32_t{}));
        REQUIRE(Variant(uint32_t{}) == Variant(uint32_t{}));
        REQUIRE(Variant(int64_t{}) == Variant(int64_t{}));
        REQUIRE(Variant(uint64_t{}) == Variant(uint64_t{}));
        REQUIRE(Variant(double{}) == Variant(double{}));
        REQUIRE(Variant(std::string{}) == Variant(std::string{}));
        REQUIRE(Variant(VariantMap{}) == Variant(VariantMap{}));
        REQUIRE(Variant(VariantVec{}) == Variant(VariantVec{}));
    }

    SECTION("from JSON") {
        SECTION("int") {
            auto const raw = R"(5)";
            Variant expected(5);
            REQUIRE(expected == Variant::from(rapidjson::Document().Parse(raw)));
        }

        SECTION("map") {
            auto const raw = R"(
                {
                    "x": 6,
                    "y": [1, 2],
                    "z": {
                        "a": "a",
                        "b": "b"
                    },
                    "a": null
                }
            )";
            Variant expected{Variant::Map{
                    std::make_pair("x", Variant(6)),
                    std::make_pair("y", Variant(Variant::Vec{Variant(1), Variant(2)})),
                    std::make_pair("z", Variant(Variant::Map{
                                                std::make_pair("a", Variant("a")),
                                                std::make_pair("b", Variant("b"))})),
                    std::make_pair("a", Variant())}};
            REQUIRE(expected == Variant::from(rapidjson::Document().Parse(raw)));
        }
    }

    SECTION("to JSON") {
        SECTION("int") {
            rapidjson::Document expected;
            expected.SetInt(6);
            rapidjson::Document json;
            Variant(int(6)).to(json);
            REQUIRE(expected == json);
        }

        SECTION("map") {
            auto const raw = R"(
                {
                    "x": 6,
                    "y": [1, 2],
                    "z": {
                        "a": "a",
                        "b": "b"
                    },
                    "a": null
                }
            )";

            rapidjson::Document expected;
            expected.Parse(raw);

            rapidjson::Document json;
            Variant{Variant::Map{
                            std::make_pair("x", Variant(6)),
                            std::make_pair("y",
                                           Variant(Variant::Vec{Variant(1), Variant(2)})),
                            std::make_pair("z",
                                           Variant(Variant::Map{
                                                   std::make_pair("a", Variant("a")),
                                                   std::make_pair("b", Variant("b"))})),
                            std::make_pair("a", Variant())}}
                    .to(json);

            REQUIRE(expected == json);
        }

        SECTION("string") {
            Variant var(VariantMap({std::make_pair("a", Variant("b"))}));
            auto const json_str = var.toJson();
            REQUIRE(json_str == R"({"a":"b"})");
            REQUIRE(var == Variant::fromJson(json_str));
            REQUIRE_THROWS_AS(Variant::fromJson("{abc"), std::runtime_error);
        }

        SECTION("char") {
            Variant const var(char(1));
            rapidjson::Document expected;
            expected.SetInt(1);
            rapidjson::Document actual;
            var.to(actual);
            REQUIRE(actual == expected);
        }

        SECTION("uchar8") {
            using uchar = unsigned char;
            Variant const var(uchar(1));
            rapidjson::Document expected;
            expected.SetUint(1);
            rapidjson::Document actual;
            var.to(actual);
            REQUIRE(actual == expected);
        }

        SECTION("int16") {
            Variant const var(int16_t(1));
            rapidjson::Document expected;
            expected.SetInt(1);
            rapidjson::Document actual;
            var.to(actual);
            REQUIRE(actual == expected);
        }

        SECTION("int16") {
            uint16_t const tmp = 1;
            Variant const var(tmp);
            rapidjson::Document expected;
            expected.SetUint(1);
            rapidjson::Document actual;
            var.to(actual);
            REQUIRE(actual == expected);
        }
    }

    SECTION("ostream") {
        using uchar = unsigned char;
        Variant var1{Variant::Map{std::make_pair("x", Variant(6))}};
        Variant var2{Variant::Map{
                std::make_pair("y", Variant(Variant::Vec{Variant(1), Variant(2)}))}};
        std::ostringstream os;
        os << Variant() << " " << Variant(true) << " " << Variant(char('1')) << " "
           << Variant(uchar('1')) << " " << Variant(int16_t(1)) << " "
           << Variant(uint16_t(1)) << " " << Variant(int32_t(1)) << " "
           << Variant(uint32_t(1)) << " " << Variant(int64_t(1)) << " "
           << Variant(uint64_t(1)) << " " << Variant(double(1)) << " " << Variant("1")
           << " " << var1 << " " << var2;
        REQUIRE(os.str() == "Null 1 1 1 1 1 1 1 1 1 1 1 { x: 6; } { y: [ 1, 2 ]; }");
    }

    SECTION("Conversion for user defined types") {
        struct X {
            static Variant toVariant(X const& x) {
                return Variant(x.x);
            }
            static X fromVariant(Variant const& x) {
                return X{x.int32()};
            }
            bool operator==(X const& rhs) const {
                return x == rhs.x;
            }
            int x;
        };

        REQUIRE(Variant(1) == Variant(X{1}));
        REQUIRE(X{1} == static_cast<X>(Variant(1)));
        REQUIRE(X{1} == X(Variant(1)));
    }

    SECTION("Assign") {
        Variant v1(1);
        Variant v2("abc");
        v2 = v1;
        REQUIRE(v2 == Variant(1));
    }

    SECTION("Comparison") {
        REQUIRE(Variant(1) == Variant(1));
        REQUIRE(Variant(2) != Variant(1));
    }

    SECTION("From JSON") {
        auto const json = R"(
            [{"abc": 1}]
        )";

        REQUIRE(Variant::from(rapidjson::Document().Parse(json)) ==
                Variant(Variant::Vec{Variant(Variant::Map{{"abc", Variant(1)}})}));
    }

    SECTION("typeInfo") {
        using uchar = unsigned char;
        REQUIRE(Variant().typeInfo() == typeid(Variant::NullType));
        REQUIRE(Variant(bool(1)).typeInfo() == typeid(bool));
        REQUIRE(Variant(char(1)).typeInfo() == typeid(char));
        REQUIRE(Variant(uchar(1)).typeInfo() == typeid(uchar));
        REQUIRE(Variant(int16_t(1)).typeInfo() == typeid(int16_t));
        REQUIRE(Variant(uint16_t(1)).typeInfo() == typeid(uint16_t));
        REQUIRE(Variant(int32_t(1)).typeInfo() == typeid(int32_t));
        REQUIRE(Variant(uint32_t(1)).typeInfo() == typeid(uint32_t));
        REQUIRE(Variant(int64_t(1)).typeInfo() == typeid(int64_t));
        REQUIRE(Variant(uint64_t(1)).typeInfo() == typeid(uint64_t));
        REQUIRE(Variant(double(1)).typeInfo() == typeid(double));
        REQUIRE(Variant("1").typeInfo() == typeid(std::string));
        REQUIRE(Variant(VariantVec()).typeInfo() == typeid(VariantVec));
        REQUIRE(Variant(VariantMap()).typeInfo() == typeid(VariantMap));
    }
}
