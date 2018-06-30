// tested
#include <variant.hpp>

#include <catch.hpp>


TEST_CASE("Check to and from integral types", "[integral]") {
    SECTION("int") {
        int const expected{5};
        auto const x = Variant(expected);
        REQUIRE(expected == x.integer());
    }
}


