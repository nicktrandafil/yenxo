// tested
#include <type_name.hpp>

// 3rd
#include <catch2/catch.hpp>


struct Def {};


namespace a { struct Xyz {}; }


struct Dum { struct Zum {}; };


TEST_CASE("Check typeName", "[utilities]") {
    REQUIRE(typeName<Def>() == "Def");
    REQUIRE(typeName<a::Xyz>() == "Xyz");
    REQUIRE(typeName<Dum::Zum>() == "Zum");
}
