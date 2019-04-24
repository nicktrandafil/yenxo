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
#include <serialize/define_struct.hpp>
#include <serialize/variant_traits.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace serialize;
using namespace boost;
using namespace boost::hana::literals;


namespace {


constexpr auto cToString(boost::hana::int_<0>) {
    return ""_s;
}

template <int I>
constexpr auto cToString(boost::hana::int_<I>) {
    constexpr auto cl = I / 10;
    return cToString(boost::hana::int_c<cl>) + boost::hana::string_c<(I % 10) + 48>;
}


struct St1 : trait::Var<St1> {
    DEFINE_STRUCT(St);
};

struct St2 : trait::Var<St2> {
    DEFINE_STRUCT(St2,
        (int, x, Default(1), Name("y"))
    );
};

struct St3 : trait::Var<St3> {
    DEFINE_STRUCT(St3,
        (int, x, Name("y"), Default(1))
    );
};

struct St4 : trait::Var<St4> {
    DEFINE_STRUCT(St4,
        (int, x, Name("y"))
    );
};

struct St5 : trait::Var<St5> {
    DEFINE_STRUCT(St5,
        (int, x, Default(1))
    );
};

struct St6 : trait::Var<St6> {
    DEFINE_STRUCT(St6,
        (int, x)
    );
};

struct St7 : trait::Var<St7> {
    DEFINE_STRUCT(St7,
        (std::string, x, Default(std::string("xx")), Name("xx"))
    );
};


struct S1 : trait::Var<S1> {
    DEFINE_STRUCT(S1,
        (int, x1, Default(1), Name("xx1"))
    );
};

struct S2 : trait::Var<S2> {
    DEFINE_STRUCT(S2,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2"))
    );
};

struct S3 : trait::Var<S3> {
    DEFINE_STRUCT(S3,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3"))
    );
};

struct S4 : trait::Var<S4> {
    DEFINE_STRUCT(S4,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4"))
    );
};

struct S5 : trait::Var<S5> {
    DEFINE_STRUCT(S5,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5"))
    );
};

struct S6 : trait::Var<S6> {
    DEFINE_STRUCT(S6,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6"))
    );
};

struct S7 : trait::Var<S7> {
    DEFINE_STRUCT(S7,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7"))
    );
};

struct S8 : trait::Var<S8> {
    DEFINE_STRUCT(S8,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8"))
    );
};

struct S9 : trait::Var<S9> {
    DEFINE_STRUCT(S9,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9"))
    );
};

struct S10 : trait::Var<S10> {
    DEFINE_STRUCT(S10,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10"))
    );
};

struct S11 : trait::Var<S11> {
    DEFINE_STRUCT(S11,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11"))
    );
};

struct S12 : trait::Var<S12> {
    DEFINE_STRUCT(S12,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12"))
    );
};

struct S13 : trait::Var<S13> {
    DEFINE_STRUCT(S13,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13"))
    );
};

struct S14 : trait::Var<S14> {
    DEFINE_STRUCT(S14,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14"))
    );
};

struct S15 : trait::Var<S15> {
    DEFINE_STRUCT(S15,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14")),
        (int, x15, Default(15), Name("xx15"))
    );
};

struct S16 : trait::Var<S16> {
    DEFINE_STRUCT(S16,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14")),
        (int, x15, Default(15), Name("xx15")),
        (int, x16, Default(16), Name("xx16"))
    );
};

struct S17 : trait::Var<S17> {
    DEFINE_STRUCT(S17,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14")),
        (int, x15, Default(15), Name("xx15")),
        (int, x16, Default(16), Name("xx16")),
        (int, x17, Default(17), Name("xx17"))
    );
};

struct S18 : trait::Var<S18> {
    DEFINE_STRUCT(S18,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14")),
        (int, x15, Default(15), Name("xx15")),
        (int, x16, Default(16), Name("xx16")),
        (int, x17, Default(17), Name("xx17")),
        (int, x18, Default(18), Name("xx18"))
    );
};

struct S19 : trait::Var<S19> {
    DEFINE_STRUCT(S19,
        (int, x1, Default(1), Name("xx1")),
        (int, x2, Default(2), Name("xx2")),
        (int, x3, Default(3), Name("xx3")),
        (int, x4, Default(4), Name("xx4")),
        (int, x5, Default(5), Name("xx5")),
        (int, x6, Default(6), Name("xx6")),
        (int, x7, Default(7), Name("xx7")),
        (int, x8, Default(8), Name("xx8")),
        (int, x9, Default(9), Name("xx9")),
        (int, x10, Default(10), Name("xx10")),
        (int, x11, Default(11), Name("xx11")),
        (int, x12, Default(12), Name("xx12")),
        (int, x13, Default(13), Name("xx13")),
        (int, x14, Default(14), Name("xx14")),
        (int, x15, Default(15), Name("xx15")),
        (int, x16, Default(16), Name("xx16")),
        (int, x17, Default(17), Name("xx17")),
        (int, x18, Default(18), Name("xx18")),
        (int, x19, Default(19), Name("xx19"))
    );
};


}


TEST_CASE("Check DEFINE_STRUCT", "[define_struct]") {
    REQUIRE(St2::defaults()["x"_s] == 1);
    REQUIRE(strcmp(St2::names()["x"_s], "y") == 0);
    REQUIRE(St3::defaults()["x"_s] == 1);
    REQUIRE(strcmp(St3::names()["x"_s], "y") == 0);
    REQUIRE(strcmp(St4::names()["x"_s], "y") == 0);
    REQUIRE(St3::defaults()["x"_s] == 1);

    REQUIRE(St7::defaults()["x"_s] == "xx");
    REQUIRE(strcmp(St7::names()["x"_s], "xx") == 0);

    auto const tests = boost::hana::make_tuple(
        boost::hana::make_pair(boost::hana::type_c<S1>, boost::hana::int_c<1>),
        boost::hana::make_pair(boost::hana::type_c<S2>, boost::hana::int_c<2>),
        boost::hana::make_pair(boost::hana::type_c<S3>, boost::hana::int_c<3>),
        boost::hana::make_pair(boost::hana::type_c<S4>, boost::hana::int_c<4>),
        boost::hana::make_pair(boost::hana::type_c<S5>, boost::hana::int_c<5>),
        boost::hana::make_pair(boost::hana::type_c<S6>, boost::hana::int_c<6>),
        boost::hana::make_pair(boost::hana::type_c<S7>, boost::hana::int_c<7>),
        boost::hana::make_pair(boost::hana::type_c<S8>, boost::hana::int_c<8>),
        boost::hana::make_pair(boost::hana::type_c<S9>, boost::hana::int_c<9>),
        boost::hana::make_pair(boost::hana::type_c<S10>, boost::hana::int_c<10>),
        boost::hana::make_pair(boost::hana::type_c<S11>, boost::hana::int_c<11>),
        boost::hana::make_pair(boost::hana::type_c<S12>, boost::hana::int_c<12>),
        boost::hana::make_pair(boost::hana::type_c<S13>, boost::hana::int_c<13>),
        boost::hana::make_pair(boost::hana::type_c<S14>, boost::hana::int_c<14>),
        boost::hana::make_pair(boost::hana::type_c<S15>, boost::hana::int_c<15>),
        boost::hana::make_pair(boost::hana::type_c<S16>, boost::hana::int_c<16>),
        boost::hana::make_pair(boost::hana::type_c<S17>, boost::hana::int_c<17>),
        boost::hana::make_pair(boost::hana::type_c<S18>, boost::hana::int_c<18>),
        boost::hana::make_pair(boost::hana::type_c<S19>, boost::hana::int_c<19>)
    );

    boost::hana::for_each(tests, boost::hana::fuse([](auto type, auto size) {
        boost::hana::for_each(boost::hana::make_range(boost::hana::int_c<1>, size + boost::hana::int_c<1>), [](auto i) {
            auto constexpr name = "x"_s + cToString(i);
            auto constexpr rename = "xx"_s + cToString(i);

            using Type = typename decltype(type)::type;
            REQUIRE(Type::defaults()[name] == i.value);
            REQUIRE(strcmp(Type::names()[name], rename.c_str()) == 0);
            REQUIRE(boost::hana::to<char const*>(Type::names()[name]) == std::string(boost::hana::to<char const*>(rename)));
        });
    }));
}
