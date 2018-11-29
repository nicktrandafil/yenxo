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
#include <rproject/type_name.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace rp;


struct Def {};


namespace a { struct Xyz {}; }


struct Dum { struct Zum {}; };


template <typename T>
struct X {};


template <bool b>
struct Y {};


enum class E { e1 };


template <E e>
struct Z {};


namespace tz {


template <E e>
struct Z {};


}


template <class T>
struct A {};


TEST_CASE("Check typeName", "[utilities]") {
    REQUIRE(unqualifiedTypeName<Def>() == "Def");
    REQUIRE(unqualifiedTypeName<a::Xyz>() == "Xyz");
    REQUIRE(unqualifiedTypeName<Dum::Zum>() == "Zum");
    REQUIRE(unqualifiedTypeName<tz::Z<E::e1>>() == "Z<(E)0>");
    REQUIRE(unqualifiedTypeName<A<Z<E::e1>>>() == "A<Z<(E)0> >");
    // FIXME
    //REQUIRE(unqualifiedTypeName<A<tz::Z<E::e1>>>() == "A<Z<(E)0> >"); 
    static_assert(unqualifiedTypeName<Def>() == "Def");

    REQUIRE(qualifiedTypeName<Def>() == "Def");
    REQUIRE(qualifiedTypeName<a::Xyz>() == "a::Xyz");
    REQUIRE(qualifiedTypeName<Dum::Zum>() == "Dum::Zum");
    static_assert(qualifiedTypeName<Def>() == "Def");

    REQUIRE(qualifiedTypeName<X<Dum>>() == "X<Dum>");
    REQUIRE(qualifiedTypeName<Y<true>>() == "Y<true>");
    REQUIRE(qualifiedTypeName<Z<E::e1>>() == "Z<(E)0>");
    REQUIRE(qualifiedTypeName<tz::Z<E::e1>>() == "tz::Z<(E)0>");
    REQUIRE(qualifiedTypeName<A<Z<E::e1>>>() == "A<Z<(E)0> >");
    REQUIRE(qualifiedTypeName<A<tz::Z<E::e1>>>() == "A<tz::Z<(E)0> >");
}
