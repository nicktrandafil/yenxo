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
#include <yenxo/define_struct.hpp>
#include <yenxo/variant_traits.hpp>

// 3rd
#include <catch2/catch.hpp>

using namespace yenxo;
using namespace boost;
using namespace boost::hana::literals;

namespace {

struct S31 : trait::Var<S31> {
    DEFINE_STRUCT(S31,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")));
};

struct S32 : trait::Var<S32> {
    DEFINE_STRUCT(S32,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")));
};

struct S33 : trait::Var<S33> {
    DEFINE_STRUCT(S33,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")),
                  (int, x33, Default(33), Name("xx33")));
};

struct S34 : trait::Var<S34> {
    DEFINE_STRUCT(S34,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")),
                  (int, x33, Default(33), Name("xx33")),
                  (int, x34, Default(34), Name("xx34")));
};

struct S35 : trait::Var<S35> {
    DEFINE_STRUCT(S35,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")),
                  (int, x33, Default(33), Name("xx33")),
                  (int, x34, Default(34), Name("xx34")),
                  (int, x35, Default(35), Name("xx35")));
};

struct S36 : trait::Var<S36> {
    DEFINE_STRUCT(S36,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")),
                  (int, x33, Default(33), Name("xx33")),
                  (int, x34, Default(34), Name("xx34")),
                  (int, x35, Default(35), Name("xx35")),
                  (int, x36, Default(36), Name("xx36")));
};

struct S37 : trait::Var<S37> {
    DEFINE_STRUCT(S37,
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
                  (int, x19, Default(19), Name("xx19")),
                  (int, x20, Default(20), Name("xx20")),
                  (int, x21, Default(21), Name("xx21")),
                  (int, x22, Default(22), Name("xx22")),
                  (int, x23, Default(23), Name("xx23")),
                  (int, x24, Default(24), Name("xx24")),
                  (int, x25, Default(25), Name("xx25")),
                  (int, x26, Default(26), Name("xx26")),
                  (int, x27, Default(27), Name("xx27")),
                  (int, x28, Default(28), Name("xx28")),
                  (int, x29, Default(29), Name("xx29")),
                  (int, x30, Default(30), Name("xx30")),
                  (int, x31, Default(31), Name("xx31")),
                  (int, x32, Default(32), Name("xx32")),
                  (int, x33, Default(33), Name("xx33")),
                  (int, x34, Default(34), Name("xx34")),
                  (int, x35, Default(35), Name("xx35")),
                  (int, x36, Default(36), Name("xx36")),
                  (int, x37, Default(37), Name("xx37")));
};

} // namespace

TEST_CASE("Check DEFINE_STRUCT part 3", "[define_struct]") {
    auto const tests = boost::hana::make_tuple(
            boost::hana::make_pair(boost::hana::type_c<S31>, boost::hana::int_c<31>),
            boost::hana::make_pair(boost::hana::type_c<S32>, boost::hana::int_c<32>),
            boost::hana::make_pair(boost::hana::type_c<S33>, boost::hana::int_c<33>),
            boost::hana::make_pair(boost::hana::type_c<S34>, boost::hana::int_c<34>),
            boost::hana::make_pair(boost::hana::type_c<S35>, boost::hana::int_c<35>),
            boost::hana::make_pair(boost::hana::type_c<S36>, boost::hana::int_c<36>),
            boost::hana::make_pair(boost::hana::type_c<S37>, boost::hana::int_c<37>));

    boost::hana::for_each(
            tests, boost::hana::fuse([](auto type, auto size) {
                boost::hana::for_each(
                        boost::hana::make_range(boost::hana::int_c<1>,
                                                size + boost::hana::int_c<1>),
                        [](auto i) {
                            auto constexpr name = "x"_s + cToString(i);
                            auto constexpr rename = "xx"_s + cToString(i);

                            using Type = typename decltype(type)::type;
                            REQUIRE(Type::defaults()[name] == i.value);
                            REQUIRE(strcmp(Type::names()[name], rename.c_str()) == 0);
                            REQUIRE(boost::hana::to<char const*>(Type::names()[name])
                                    == std::string(boost::hana::to<char const*>(rename)));
                        });
            }));
}
