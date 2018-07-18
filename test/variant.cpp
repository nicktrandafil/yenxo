// tested
#include <variant.hpp>

// 3rd
#include <catch.hpp>


TEST_CASE("Check to and from types", "[to_from]") {
    SECTION("integer") {
        int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.integer());
        REQUIRE_THROWS_AS(Variant().integer(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").integer(), VariantBadType);
        REQUIRE(Variant().integerOr(1) == 1);
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

    SECTION("Variant::type") {
        REQUIRE(Variant::TypeId::Int == Variant(5).type());
        REQUIRE(Variant::TypeId::Map == Variant(Variant::Map()).type());
        REQUIRE(Variant::TypeId::No == Variant().type());
        REQUIRE(Variant::TypeId::Str == Variant("").type());
        REQUIRE(Variant::TypeId::Vec == Variant(Variant::Vec()).type());
    }
}


