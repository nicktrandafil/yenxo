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


// tested
#include <variant.hpp>
#include <variant_traits.hpp>
#include <comparison_traits.hpp>
#include <ostream_traits.hpp>

// 3rd
#include <catch2/catch.hpp>


namespace hana = boost::hana;


namespace {


struct Protagonist1 {};


struct Protagonist2 {
    static bool fromVariant(int const& x);
    static bool toVariant(int const& x);
};


struct Protagonist3 {
    static bool fromVariant(Variant const&);
    static bool toVariant(Protagonist3);
};


struct Film1 : trait::Var<Film1> {
    Protagonist1 proto;
};


struct Film2 : trait::Var<Film2> {
    Protagonist2 proto;
};


struct Film3 : trait::Var<Film3> {
    Protagonist3 proto;
};


} //namespace


BOOST_HANA_ADAPT_STRUCT(Film1, proto);
BOOST_HANA_ADAPT_STRUCT(Film2, proto);
BOOST_HANA_ADAPT_STRUCT(Film3, proto);


TEST_CASE("Check error diagnostic, [diagnostic]") {
//    Film1::fromVariant(Variant());
//    Film2::fromVariant(Variant());
//    Film3::fromVariant(Variant());

//    Film1::toVariant(Film1());
//    Film2::toVariant(Film2());
//    Film3::toVariant(Film3());
}
