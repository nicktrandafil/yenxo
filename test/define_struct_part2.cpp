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


struct S20 : trait::Var<S20> {
    DEFINE_STRUCT(S20,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s))
    );
};

struct S21 : trait::Var<S21> {
    DEFINE_STRUCT(S21,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s))
    );
};

struct S22 : trait::Var<S22> {
    DEFINE_STRUCT(S22,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s))
    );
};

struct S23 : trait::Var<S23> {
    DEFINE_STRUCT(S23,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s))
    );
};

struct S24 : trait::Var<S24> {
    DEFINE_STRUCT(S24,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s))
    );
};

struct S25 : trait::Var<S25> {
    DEFINE_STRUCT(S25,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s))
    );
};

struct S26 : trait::Var<S26> {
    DEFINE_STRUCT(S26,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s)),
        (int, x26, Default(26), Name("xx26"_s))
    );
};

struct S27 : trait::Var<S27> {
    DEFINE_STRUCT(S27,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s)),
        (int, x26, Default(26), Name("xx26"_s)),
        (int, x27, Default(27), Name("xx27"_s))
    );
};

struct S28 : trait::Var<S28> {
    DEFINE_STRUCT(S28,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s)),
        (int, x26, Default(26), Name("xx26"_s)),
        (int, x27, Default(27), Name("xx27"_s)),
        (int, x28, Default(28), Name("xx28"_s))
    );
};

struct S29 : trait::Var<S29> {
    DEFINE_STRUCT(S29,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s)),
        (int, x26, Default(26), Name("xx26"_s)),
        (int, x27, Default(27), Name("xx27"_s)),
        (int, x28, Default(28), Name("xx28"_s)),
        (int, x29, Default(29), Name("xx29"_s))
    );
};

struct S30 : trait::Var<S30> {
    DEFINE_STRUCT(S30,
        (int, x1, Default(1), Name("xx1"_s)),
        (int, x2, Default(2), Name("xx2"_s)),
        (int, x3, Default(3), Name("xx3"_s)),
        (int, x4, Default(4), Name("xx4"_s)),
        (int, x5, Default(5), Name("xx5"_s)),
        (int, x6, Default(6), Name("xx6"_s)),
        (int, x7, Default(7), Name("xx7"_s)),
        (int, x8, Default(8), Name("xx8"_s)),
        (int, x9, Default(9), Name("xx9"_s)),
        (int, x10, Default(10), Name("xx10"_s)),
        (int, x11, Default(11), Name("xx11"_s)),
        (int, x12, Default(12), Name("xx12"_s)),
        (int, x13, Default(13), Name("xx13"_s)),
        (int, x14, Default(14), Name("xx14"_s)),
        (int, x15, Default(15), Name("xx15"_s)),
        (int, x16, Default(16), Name("xx16"_s)),
        (int, x17, Default(17), Name("xx17"_s)),
        (int, x18, Default(18), Name("xx18"_s)),
        (int, x19, Default(19), Name("xx19"_s)),
        (int, x20, Default(20), Name("xx20"_s)),
        (int, x21, Default(21), Name("xx21"_s)),
        (int, x22, Default(22), Name("xx22"_s)),
        (int, x23, Default(23), Name("xx23"_s)),
        (int, x24, Default(24), Name("xx24"_s)),
        (int, x25, Default(25), Name("xx25"_s)),
        (int, x26, Default(26), Name("xx26"_s)),
        (int, x27, Default(27), Name("xx27"_s)),
        (int, x28, Default(28), Name("xx28"_s)),
        (int, x29, Default(29), Name("xx29"_s)),
        (int, x30, Default(30), Name("xx30"_s))
    );
};


}


TEST_CASE("Check DEFINE_STRUCT part 2", "[define_struct]") {
    auto const tests = boost::hana::make_tuple(
        boost::hana::make_pair(boost::hana::type_c<S20>, boost::hana::int_c<20>),
        boost::hana::make_pair(boost::hana::type_c<S21>, boost::hana::int_c<21>),
        boost::hana::make_pair(boost::hana::type_c<S22>, boost::hana::int_c<22>),
        boost::hana::make_pair(boost::hana::type_c<S23>, boost::hana::int_c<23>),
        boost::hana::make_pair(boost::hana::type_c<S24>, boost::hana::int_c<24>),
        boost::hana::make_pair(boost::hana::type_c<S25>, boost::hana::int_c<25>),
        boost::hana::make_pair(boost::hana::type_c<S26>, boost::hana::int_c<26>),
        boost::hana::make_pair(boost::hana::type_c<S27>, boost::hana::int_c<27>),
        boost::hana::make_pair(boost::hana::type_c<S28>, boost::hana::int_c<28>),
        boost::hana::make_pair(boost::hana::type_c<S29>, boost::hana::int_c<29>),
        boost::hana::make_pair(boost::hana::type_c<S30>, boost::hana::int_c<30>)
    );

    boost::hana::for_each(tests, boost::hana::fuse([](auto type, auto size) {
        boost::hana::for_each(boost::hana::make_range(boost::hana::int_c<1>, size + boost::hana::int_c<1>), [](auto i) {
            auto constexpr name = "x"_s + cToString(i);
            auto constexpr rename = "xx"_s + cToString(i);

            using Type = typename decltype(type)::type;
            REQUIRE(Type::defaults()[name] == i.value);
            BOOST_HANA_CONSTANT_ASSERT(Type::names()[name] == rename);
            REQUIRE(boost::hana::to<char const*>(Type::names()[name]) == std::string(boost::hana::to<char const*>(rename)));
        });
    }));
}
