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
#include <variant_mixins.hpp>

// local
#include <comparison_mixins.hpp>
#include <ostream_mixins.hpp>

// 3rd
#include <catch2/catch.hpp>


namespace {


struct UserDefinedStr : private std::string {
    UserDefinedStr() = default;
    UserDefinedStr(std::string const& x) : std::string(x) {}

    bool operator==(UserDefinedStr const& rhs) const {
        return static_cast<std::string const&>(*this) ==
                static_cast<std::string const&>(rhs);
    }

    friend std::ostream& operator<<(std::ostream& out,
                                    UserDefinedStr const& x) {
        return out << static_cast<std::string const&>(x);
    }
};


} // namespace


namespace mixin::detail {


template <>
struct FromVariantImpl<UserDefinedStr> {
    static UserDefinedStr apply(Variant const& x) {
        return "User defined " + x.str();
    }
};


} // namespace mixin::detail


namespace {


struct Hobby
        : mixin::Var<Hobby>
        , mixin::EqualityComparison<Hobby>
        , mixin::OStream<Hobby> {
    Hobby() = default;
    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    int id{0};
    UserDefinedStr description;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);


TEST_CASE("Check mixin::Var redefine", "[variant_mixins]") {
    Variant::Map map{
        {"id", Variant(9)},
        {"description", Variant("abc")}
    };

    REQUIRE(Hobby::fromVariant(Variant(map)) == Hobby(9, "User defined abc"));
}
