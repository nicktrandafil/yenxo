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


// tested
#include <serialize/variant_conversion.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace serialize;


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
};


[[maybe_unused]] ETraits traits(E) { return {}; }


}


TEST_CASE("Check toVariant/fromVariant", "[variant_conversion]") {
    REQUIRE(toVariant(E::e1) == Variant("e1"));
    REQUIRE(fromVariant<E>(Variant("e2")) == E::e2);
}
