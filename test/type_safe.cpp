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
#include <variant_mixins.hpp>
#include <comparison_mixins.hpp>

#include <type_safe/strong_typedef.hpp>
#include <type_safe/constrained_type.hpp>
#include <type_safe/types.hpp>

// 3rd
#include <catch2/catch.hpp>


namespace ts = type_safe;


namespace {


// Centimetre typedef
class Centimetre :
        public ts::strong_typedef<Centimetre, int>,
        public ts::strong_typedef_op::equality_comparison<Centimetre>,
        public ts::strong_typedef_op::relational_comparison<Centimetre>,
        public ts::strong_typedef_op::integer_arithmetic<Centimetre> {
public:
    using strong_typedef::strong_typedef;
};


struct Table
        : mixin::Var<Table>
        , mixin::EqualityComparison<Table> {
    Table() = default;
    Table(Centimetre w, Centimetre d) : width(w), depth(d) {}
    Centimetre width;
    Centimetre depth;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Table, width, depth);


TEST_CASE("Check mixin::Var with type_safe::strong_typedef",
          "[variant_mixins]") {
    Table const t{
        Centimetre(100),
        Centimetre(200)
    };

    Variant::Map v{
        {"width", Variant(100)},
        {"depth", Variant(200)}
    };

    REQUIRE(Table::fromVariant(Variant(v)) == t);
    REQUIRE(Table::toVariant(t) == Variant(v));
}


namespace {


using non_empty_string = ts::constrained_type<std::string, ts::constraints::non_empty>;


struct Pen
        : mixin::Var<Pen>
        , mixin::EqualityComparison<Pen> {
    Pen() = default;
    explicit Pen(non_empty_string const& color) : color(color) {}

    non_empty_string color{"green"};
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Pen, color);


TEST_CASE("Check mixin::Var with type_safe::constraint_type",
          "[variant_mixins]") {
    Variant::Map const v{
        {"color", Variant("red")}
    };

    Pen const pen{
        non_empty_string("red")
    };

    REQUIRE(Pen::fromVariant(Variant(v)) == pen);
    REQUIRE(Pen::toVariant(pen) == Variant(v));
}


namespace {


struct RemoteControl : mixin::Var<RemoteControl> {
    RemoteControl() = default;
    RemoteControl(
            ts::unsigned_t bc,
            ts::double_t f,
            ts::bool_t o)
        : buttons_count(bc)
        , frequency(f)
        , is_on(o)
    {}

    bool operator==(RemoteControl const& rhs) const {
        return buttons_count == rhs.buttons_count &&
                double(frequency) == double(rhs.frequency) &&
                is_on == rhs.is_on;
    }

    ts::unsigned_t buttons_count{0u};
    ts::double_t frequency{0.0};
    ts::bool_t is_on{false};
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(RemoteControl, buttons_count, frequency, is_on);


TEST_CASE("Check mixin::Var with type_safe build-in types",
          "[variant_mixins]") {
    Variant::Map const v{
        {"buttons_count", Variant(2u)},
        {"frequency", Variant(2.2)},
        {"is_on", Variant(true)}
    };

    RemoteControl const rc{
        ts::unsigned_t(2u),
        ts::double_t(2.2),
        ts::bool_t(true)
    };

    REQUIRE(RemoteControl::fromVariant(Variant(v)) == rc);
    REQUIRE(RemoteControl::toVariant(rc) == Variant(v));
}
