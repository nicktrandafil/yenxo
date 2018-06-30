// tested
#include <variant.hpp>

// 3rd
#include <catch.hpp>


TEST_CASE("Check to and from integral types", "[integral]") {
    SECTION("int") {
        int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.integer());
        REQUIRE_THROWS_AS(Variant().integer(), VariantEmpty);
        REQUIRE_THROWS_AS(Variant("").integer(), VariantBadType);
        REQUIRE(Variant().integerOr(1) == 1);
    }
}


