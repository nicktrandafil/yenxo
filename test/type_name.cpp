// tested
#include <type_name.hpp>

// 3rd
#include <catch2/catch.hpp>


struct Def {};


namespace a { struct Xyz {}; }


struct Dum { struct Zum {}; };


TEST_CASE("Check typeName", "[utilities]") {
    REQUIRE(unqualifiedTypeName<Def>() == "Def");
    REQUIRE(unqualifiedTypeName<a::Xyz>() == "Xyz");
    REQUIRE(unqualifiedTypeName<Dum::Zum>() == "Zum");
}
