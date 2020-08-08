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

#include <serialize/comparison_traits.hpp>
#include <serialize/ostream_traits.hpp>
#include <serialize/string_conversion.hpp>
#include <serialize/value_tag.hpp>
#include <serialize/variant.hpp>
#include <serialize/variant_traits.hpp>

#include <catch2/catch.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

using namespace serialize;

using namespace hana::literals;
using namespace std::literals;

namespace {

struct X {
    static Variant toVariant(X const&);
    static X fromVariant(Variant const&);
};
static_assert(hasFromVariant(boost::hana::type_c<X>));
static_assert(hasToVariant(boost::hana::type_c<X>));

struct Hobby
        : trait::Var<Hobby>
        , trait::UpdateFromVar<Hobby>
        , trait::EqualityComparison<Hobby> {
    Hobby()
        : id(0) {}

    Hobby(int id, std::string const& description)
        : id(id), description(description) {}

    int id;
    std::string description;
};

struct OptPerson : trait::EqualityComparison<OptPerson> {
    OptPerson() = default;

    OptPerson(std::string const& name, int age, Hobby const& hobby)
            : name(name)
            , age(age)
            , hobby(hobby) {
    }

    std::optional<std::string> name;
    std::optional<int> age;
    std::optional<Hobby> hobby;
};

struct Person
        : trait::Var<Person>
        , trait::UpdateFromVar<Person>
        , trait::UpdateFromOpt<Person, OptPerson>
        , trait::EqualityComparison<Person> {
    Person()
        : age(0) {}

    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby) {}

    std::string name;
    int age;
    Hobby hobby;
};

struct AllowAdditionalPropertiesPolicy : trait::VarDefPolicy {
    static auto constexpr allow_additional_properties = false;
};

struct PersonNoAdditionalProperties
        : trait::UpdateFromVar<PersonNoAdditionalProperties,
                               AllowAdditionalPropertiesPolicy>
        , trait::EqualityComparison<PersonNoAdditionalProperties> {
    PersonNoAdditionalProperties() = default;
    PersonNoAdditionalProperties(std::string const& name, int age, Hobby const& hobby)
            : name(name)
            , age(age)
            , hobby(hobby) {
    }
    BOOST_HANA_DEFINE_STRUCT(PersonNoAdditionalProperties,
        (std::string, name),
        (int        , age),
        (Hobby      , hobby)
    );
};

struct PersonWithVector
        : trait::Var<PersonWithVector>
        , trait::EqualityComparison<PersonWithVector> {
    PersonWithVector() = default;
    PersonWithVector(std::string const& name, std::vector<Hobby> const& hobbies)
            : name(name)
            , hobbies(hobbies) {
    }

    std::string name;
    std::vector<Hobby> hobbies;
};

struct Dict
    : trait::Var<Dict>,
      trait::EqualityComparison<Dict> {
    Dict() = default;

    Dict(int x, std::unordered_map<std::string, int> const& map)
        : x(x), map(map) {}

    int x;
    std::unordered_map<std::string, int> map;
};

} // namespace

BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(OptPerson, name, age, hobby);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
BOOST_HANA_ADAPT_STRUCT(PersonWithVector, name, hobbies);
BOOST_HANA_ADAPT_STRUCT(Dict, x, map);

TEST_CASE("detail::toVariant", "[variant_trait_helpers]") {
    auto const x = toVariant(5);
    REQUIRE(x == Variant(5));
    REQUIRE(5 == fromVariant<int>(Variant(5)));

    auto const x1 = toVariant("Hello");
    REQUIRE(x1 == Variant("Hello"));
    REQUIRE("Hello" == fromVariant<std::string>(Variant("Hello")));

    struct X {
        static Variant toVariant(X const& x) { return Variant(x.m); }
        static X fromVariant(Variant const& x) { return {x.int32()}; }
        int m{5};
    };

    auto const x2 = toVariant(X());
    REQUIRE(x2 == Variant(5));
    REQUIRE(6 == fromVariant<X>(Variant(6)).m);

    SECTION("quick struct") {
        Variant::Map const m{std::make_pair("a", Variant(1))};
        auto hm = hana::make_map(hana::make_pair("a"_s, 1));
        auto const hm_v = toVariant(hm);
        REQUIRE(hm_v == Variant(m));
        REQUIRE(boost::hana::equal(fromVariant<decltype(hm)>(Variant(m)), hm));
        auto hm2 = hana::make_map(hana::make_pair("b"_s, 2));
        REQUIRE_THROWS_WITH(fromVariant<decltype(hm2)>(Variant(m)),
                            "b is required");
    }

    SECTION("integral constant") {
        Variant ok(2);
        Variant not_ok(3);
        std::integral_constant<int, 2> c;
        REQUIRE(fromVariant<decltype(c)>(ok) == c);
        REQUIRE_THROWS_AS(fromVariant<decltype(c)>(not_ok), VariantBadType);
        REQUIRE(toVariant(c) == Variant(2));
    }
}

TEST_CASE("Check trait::Var and trait::UpdateFromVar", "[variant_traits]") {
    SECTION("optional") {
        struct Opt
                : trait::Var<Opt>
                , trait::EqualityComparison<Opt> {
            Opt() = default;
            Opt(int x)
                    : x(x) {
            }

            BOOST_HANA_DEFINE_STRUCT(Opt, (std::optional<int>, x));
        };

        Variant const expected(VariantMap{{"x", Variant(1)}});
        REQUIRE(Opt(1) == fromVariant<Opt>(expected));
    }

    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}};

    Variant::Map person_var{
        {"name", Variant("Alecu")},
        {"age", Variant(25)},
        {"hobby", Variant(hobby_var)}};

    Hobby const hobby{
        1,
        std::string("Hack")};

    Person person{
        "Alecu",
        25,
        hobby};

    SECTION("Check fromVariant") {
        REQUIRE(person == Person::fromVariant(Variant(person_var)));
    }

    SECTION("Check toVariant") {
        REQUIRE(Person::toVariant(person) == Variant(person_var));
    }

    SECTION("Check updateVar") {
        Variant::Map person_var{
            {"age", Variant(26)}};

        person.updateVar(Variant(person_var));

        Person const person_updated{
            "Alecu",
            26,
            hobby};

        REQUIRE(person_updated == person);

        person_var["no"] = Variant(1);
        REQUIRE_NOTHROW(person.updateVar(Variant(person_var)));
    }

    SECTION("Check updateVar with allow_additional_properties=false") {
        PersonNoAdditionalProperties person{"Name", 25, hobby};
        PersonNoAdditionalProperties const person_updated{"Name", 26, hobby};
        Variant::Map person_var{{"age", Variant(26)}};
        person.updateVar(Variant(person_var));
        REQUIRE(person_updated == person);
        person_var["no"] = Variant(1);
        REQUIRE_THROWS_WITH(person.updateVar(Variant(person_var)), "'no' is unknown");
    }

    SECTION("Check updateVar with nested struct") {
        struct Parent
                : trait::UpdateFromVar<Parent>
                , trait::EqualityComparison<Parent> {
            Parent()
                    : name("a")
                    , age(30) {
            }
            BOOST_HANA_DEFINE_STRUCT(Parent, (std::string, name), (int, age));
        };

        struct Person
                : trait::UpdateFromVar<Person>
                , trait::EqualityComparison<Person> {
            Person()
                    : age(9) {
            }
            BOOST_HANA_DEFINE_STRUCT(Person, (int, age), (Parent, parent));
        };

        auto const var = Variant::fromJson(R"(
            {
                "age": 10,
                "parent": {
                    "name": "b"
                }
            })");

        Person person;
        person.parent.age = 99;

        Person person_expected;
        person_expected.parent.age = 99;
        person_expected.age = 10;
        person_expected.parent.name = "b";

        person.updateVar(Variant(var));
        REQUIRE(person == person_expected);
    }

    SECTION("Check updateOpt") {
        OptPerson opt;
        opt.age = 27;

        person.updateOpt(opt);

        Person const person_updated{
            "Alecu",
            27,
            hobby};

        REQUIRE(person_updated == person);
    }

    SECTION("Check no field") {
        person_var.erase("name");
        REQUIRE_THROWS_AS(
            Person::fromVariant(Variant(person_var)),
            std::logic_error);
    }

    Hobby hobby2{
        2,
        "Barista"};

    Variant const hobby2_var{Variant::Map{
        {"id", Variant(2)},
        {"description", Variant("Barista")}}};

    PersonWithVector const person_ex{"P", std::vector<Hobby>{hobby, hobby2}};

    Variant person_ex_var{Variant::Map{
        std::make_pair("name", Variant("P")),
        std::make_pair("hobbies", Variant(Variant::Vec{Variant(hobby_var), hobby2_var}))}};

    SECTION("Check struct with vector") {
        REQUIRE(person_ex == PersonWithVector::fromVariant(person_ex_var));
        REQUIRE(person_ex_var == PersonWithVector::toVariant(person_ex));
    }

    SECTION("Check dict") {
        Dict const expected{
            5,
            {{"1", 1}, {"2", 2}}};

        Variant const tmp{Variant::Map{
            {"x", Variant(5)},
            {"map", Variant({{"1", Variant(1)}, {"2", Variant(2)}})}}};

        REQUIRE(expected == Dict::fromVariant(tmp));
        REQUIRE(tmp == Dict::toVariant(expected));
    }

    SECTION("rename") {
        struct Rename
                : trait::Var<Rename>
                , trait::EqualityComparison<Rename> {
            Rename() = default;
            Rename(int x)
                    : x(x) {
            }
            constexpr static auto names() {
                return hana::make_map(hana::make_pair("x"_s, "y"));
            }
            BOOST_HANA_DEFINE_STRUCT(Rename, (int, x));
        };

        Variant expected(VariantMap{{"y", Variant(2)}});
        Variant wrong(VariantMap{{"z", Variant(2)}});
        REQUIRE(fromVariant<Rename>(expected) == Rename(2));
        REQUIRE(toVariant(Rename(2)) == expected);
        REQUIRE_THROWS_AS(fromVariant<Rename>(wrong), std::logic_error);
        REQUIRE_THROWS_WITH(fromVariant<Rename>(wrong), "'y' is required");
    }
}

namespace {

struct PersonD
    : trait::VarDef<PersonD>,
      trait::EqualityComparison<PersonD> {
    PersonD() {}

    PersonD(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby) {}

    PersonD(std::string const& name, Hobby const& hobby)
        : name(name), hobby(hobby) {}

    static auto defaults() {
        return hana::make_map(
            hana::make_pair("name"_s, "Efendi"s));
    }

    std::string name;
    std::optional<int> age;
    Hobby hobby;
};

struct Pol : trait::VarDefPolicy {
    static auto constexpr serialize_default_value = false;
    static auto constexpr empty_container_not_required = true;
};

struct Pol2 : trait::VarDefPolicy {
    static auto constexpr empty_container_not_required = true;
};

struct Pol3 : trait::VarDefPolicy {
    static auto constexpr from_variant = [](auto& x, Variant const& var) { x = serialize::fromString<std::remove_reference_t<decltype(x)>>(var.str()); };
    static auto constexpr to_variant = [](Variant& var, auto&& x) { var = Variant(serialize::toString(std::forward<std::remove_reference_t<decltype(x)>>(x))); };
};

struct PersonE
    : trait::VarDef<PersonE, Pol> {

    static constexpr auto defaults() {
        return hana::make_map(hana::make_pair("i"_s, 2));
    }

    PersonE() = default;
    std::vector<int> cont;
    int i{0};
};

struct PersonE2
    : trait::VarDef<PersonE2, Pol2> {

    static constexpr auto defaults() {
        return hana::make_map(hana::make_pair("i"_s, 2));
    }

    PersonE2()
        : i(0) {}

    BOOST_HANA_DEFINE_STRUCT(PersonE2,
                             (std::vector<int>, cont),
                             (int, i),
                             (std::string, s));
};

struct St3 : trait::VarDef<St3>, trait::EqualityComparison<St3> {
    St3() = default;
    St3(int x)
        : x(x) {}

    constexpr static auto names() {
        return hana::make_map(
            hana::make_pair("x"_s, "y"));
    }

    BOOST_HANA_DEFINE_STRUCT(St3,
                             (int, x));
};

struct S1 {
    S1() = default;
    explicit S1(std::string const& str)
        : v(str) {}
    explicit operator std::string const&() const noexcept { return v; }
    bool operator==(S1 const& rhs) const noexcept { return v == rhs.v; }
    std::string v;
};

struct StrStruct
    : trait::VarDef<StrStruct, Pol3>,
      trait::EqualityComparison<StrStruct> {
    StrStruct() = default;
    explicit StrStruct(std::string const& str)
        : str(str) {}
    BOOST_HANA_DEFINE_STRUCT(StrStruct,
                             (S1, str));
};

struct CamelCase {
    template <class T, class S>
    std::string operator()(boost::hana::basic_type<T>, S name) const {
        std::string renamed = boost::hana::to<char const*>(name);
        assert(renamed.size());
        renamed[0] = char(std::tolower(renamed[0]));
        for (size_t i = 0; i < renamed.size();) {
            if (renamed[i] == '_' && i + 1 < renamed.size()) {
                renamed[i + 1] = char(toupper(renamed[i + 1]));
                renamed.erase(i, 1);
            } else {
                ++i;
            }
        }
        return renamed;
    }
};

struct CamelCasePolicy : trait::VarDefPolicy {
    static constexpr CamelCase rename{};
};

struct AutoCamelCase : trait::VarDef<AutoCamelCase, CamelCasePolicy>,
                       trait::EqualityComparison<AutoCamelCase> {
    BOOST_HANA_DEFINE_STRUCT(AutoCamelCase,
                             (std::string, hello_world_2_boo));
};

struct AdditionalProp
        : trait::VarDef<AdditionalProp, AllowAdditionalPropertiesPolicy>
        , trait::EqualityComparison<AdditionalProp> {
    constexpr static auto names() {
        return hana::make_map(
            hana::make_pair("y"_s, "z"));
    }
    BOOST_HANA_DEFINE_STRUCT(AdditionalProp,
                             (int, x),
                             (int, y));
};

} // namespace

BOOST_HANA_ADAPT_STRUCT(PersonD, name, age, hobby);
BOOST_HANA_ADAPT_STRUCT(PersonE, cont, i);

TEST_CASE("Check trait::VarDef", "[variant_traits]") {
    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}};

    Variant::Map person_var{
        {"name", Variant("Alecu")},
        {"age", Variant(16)},
        {"hobby", Variant(hobby_var)}};

    Hobby const hobby{
        1,
        std::string("Hack")};

    PersonD person{
        "Alecu",
        16,
        hobby};

    SECTION("Check toVariant with default value (should serialize)") {
        person.name = "Efendi";
        person_var["name"] = Variant("Efendi");
        REQUIRE(Variant(person) == Variant(person_var));
    }

    SECTION("Check toVariant with uninitialized age") {
        person_var.erase("age");
        person.age.reset();
        REQUIRE(PersonD::toVariant(person) == Variant(person_var));
    }

    SECTION("Check toVariant with initialized age") {
        REQUIRE(PersonD::toVariant(person) == Variant(person_var));
    }

    SECTION("Check fromVariant with no name (default value is provided)") {
        person_var.erase("name");
        auto const actual = PersonD::fromVariant(Variant(person_var));
        REQUIRE(actual == PersonD{"Efendi", 16, hobby});
    }

    SECTION("Check fromVariant with no age (no default value, but type is optional)") {
        person_var.erase("age");
        auto const actual = PersonD::fromVariant(Variant(person_var));
        REQUIRE(actual == PersonD{"Alecu", hobby});
    }

    SECTION("Check fromVariant with no hobby (no default value)") {
        person_var.erase("hobby");
        REQUIRE_THROWS_AS(PersonD::fromVariant(Variant(person_var)), std::logic_error);
    }

    SECTION("Check from_variant policy parameter") {
        Variant const var(
            VariantMap{
                {"str", Variant("1")}});
        REQUIRE(fromVariant<StrStruct>(var) == StrStruct("1"));
        REQUIRE(var == toVariant(StrStruct("1")));
    }

    SECTION("Check toVariant policy redefinition") {
        PersonE p1;
        p1.i = 2;
        REQUIRE(Variant(p1) == Variant(VariantMap{}));

        PersonE p2;
        p2.i = 0;
        p2.cont.push_back(1);
        VariantMap exp{
            {"i", Variant(0)},
            {"cont", Variant(VariantVec{Variant(1)})}};
        REQUIRE(Variant(p2) == Variant(exp));

        PersonE2 p3;
        p3.i = 2;
        REQUIRE(Variant(p3) == Variant(VariantMap{{"i", Variant(2)}, {"s", Variant("")}}));
    }

    SECTION("default rename") {
        Variant expected(VariantMap{{"y", Variant(2)}});
        Variant wrong(VariantMap{{"z", Variant(2)}});
        REQUIRE(fromVariant<St3>(expected) == St3(2));
        REQUIRE(toVariant(St3(2)) == expected);
        REQUIRE_THROWS_AS(fromVariant<St3>(wrong), std::logic_error);
        REQUIRE_THROWS_WITH(fromVariant<St3>(wrong), "'y' is required");
    }

    SECTION("rename policy") {
        Variant var(VariantMap{{"helloWorld2Boo", Variant("")}});
        AutoCamelCase st;
        REQUIRE(toVariant(st) == var);
        var.modifyMap().begin()->second = Variant("a");
        st.hello_world_2_boo = "a";
        REQUIRE(fromVariant<AutoCamelCase>(var) == st);
    }

    SECTION("additional prop policy") {
        Variant var(VariantMap{{"x", Variant(1)}, {"z", Variant(2)}});

        AdditionalProp expected;
        expected.x = 1;
        expected.y = 2;

        AdditionalProp p;
        REQUIRE_NOTHROW(fromVariant2(p, var));
        REQUIRE(expected == p);

        var.modifyMap()["u"] = Variant(3);
        REQUIRE_THROWS_AS(fromVariant2(p, var), std::logic_error);
        REQUIRE_THROWS_WITH(fromVariant2(p, var), "'u' is unknown");
    }
}

namespace {

struct Car : trait::Var<Car> {
    Car(std::string const& name, int wheels)
        : name(name), wheels(wheels) {}
    Car() = default;

    std::string name;
    int wheels{0};
};

} // namespace

BOOST_HANA_ADAPT_STRUCT(Car, name, wheels);

TEST_CASE("Check trait::Var fails", "[variant_traits]") {
    Variant car_var{Variant::Map{
        {"name", Variant("saab")},
        {"wheels", Variant("4")}}};

    REQUIRE_THROWS_AS(Car::fromVariant(car_var), VariantBadType);
    REQUIRE_THROWS_WITH(Car::fromVariant(car_var), ".wheels: expected 'int32', actual 'string'");
    REQUIRE(hana::equal(int(1), int(1)));
}

namespace {

struct TagPolicy : trait::VarDefPolicy {
    static constexpr auto tag = "a tag"_s;
};

struct Tagged : trait::VarDef<Tagged, TagPolicy> {
    Tagged() = default;
    explicit Tagged(int x) : value(x) {
    }
    BOOST_HANA_DEFINE_STRUCT(Tagged,
        (int, value)
    );
};

} // namespace

TEST_CASE("Check Policy::Tag", "[variant_traits]") {
    Variant var(VariantMap{{"__tag", Variant("a tag")}, {"value", Variant(10)}});

    SECTION("ok") {
        Tagged const val(10);
        REQUIRE(toVariant(Tagged(10)) == var);
        REQUIRE(val.value == fromVariant<Tagged>(var).value);
    }

    SECTION("fail") {
        var.modifyMap().at("__tag") = Variant("foo");
        REQUIRE_THROWS_WITH(fromVariant<Tagged>(var),
                            ".__tag: 'foo' is not of type 'a tag literal'");
    }
}
