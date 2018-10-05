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
#include <rproject/variant_traits.hpp>

// local
#include <rproject/ostream_traits.hpp>
#include <rproject/comparison_traits.hpp>
#include <rproject/variant.hpp>

// 3rd
#include <catch2/catch.hpp>

// boost
#include <boost/hana.hpp>


namespace hana = boost::hana;


using namespace hana::literals;
using namespace std::literals;


namespace {


struct X {
    static Variant toVariant(X const&);
    static X fromVariant(Variant const&);
};


static_assert(trait::hasFromVariant(rp::type_c<X>));
static_assert(trait::hasToVariant(rp::type_c<X>));


struct Hobby
        : trait::Var<Hobby>
        , trait::UpdateFromVar<Hobby>
        , trait::OStream<Hobby>
        , trait::EqualityComparison<Hobby> {

    Hobby() : id(0) {}

    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    int id;
    std::string description;
};


struct Person
        : trait::Var<Person>
        , trait::UpdateFromVar<Person>
        , trait::OStream<Person>
        , trait::EqualityComparison<Person> {

    Person() : age(0) {}

    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    std::string name;
    int age;
    Hobby hobby;
};


struct PersonEx
        : trait::Var<PersonEx>
        , trait::OStream<PersonEx>
        , trait::EqualityComparison<PersonEx> {

    PersonEx() {}
    PersonEx(std::string const& name, std::vector<Hobby> const& hobbies)
        : name(name), hobbies(hobbies)
    {}

    std::string name;
    std::vector<Hobby> hobbies;
};


struct Dict
        : trait::Var<Dict>
        , trait::EqualityComparison<Dict>
        , trait::OStream<Dict> {
    Dict() = default;

    Dict(int x, std::unordered_map<std::string, int> const& map)
        : x(x), map(map)
    {}

    int x;
    std::unordered_map<std::string, int> map;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
BOOST_HANA_ADAPT_STRUCT(PersonEx, name, hobbies);
BOOST_HANA_ADAPT_STRUCT(Dict, x, map);


TEST_CASE("detail::toVariant", "[variant_trait_helpers]") {
    auto const x = trait::toVariant(5);
    REQUIRE(x == Variant(5));
    REQUIRE(5 == trait::fromVariant<int>(Variant(5)));

    auto const x1 = trait::toVariant("Hello");
    REQUIRE(x1 == Variant("Hello"));
    REQUIRE("Hello" ==trait::fromVariant<std::string>(Variant("Hello")));

    struct X {
        static Variant toVariant(X const& x) { return Variant(x.m); }
        static X fromVariant(Variant const& x) { return {x.integer()}; }
        int m{5};
    };

    auto const x2 = trait::toVariant(X());
    REQUIRE(x2 == Variant(5));
    REQUIRE(6 == trait::fromVariant<X>(Variant(6)).m);
}


TEST_CASE("Check trait::Var and trait::UpdateFromVar", "[variant_traits]") {
    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}
    };

    Variant::Map person_var{
        {"name", Variant("Alecu")},
        {"age", Variant(25)},
        {"hobby", Variant(hobby_var)}
    };

    Hobby const hobby{
        1,
        std::string("Hack")
    };

    Person person{
        "Alecu",
        25,
        hobby
    };

    SECTION("Check fromVariant") {
        REQUIRE(person == Person::fromVariant(Variant(person_var)));
    }

    SECTION("Check toVariant") {
        REQUIRE(Person::toVariant(person) == Variant(person_var));
    }

    SECTION("Check update") {
        Variant::Map const person_var{
            {"age", Variant(26)}
        };

        person.update(Variant(person_var));

        Person const person_updated{
            "Alecu",
            26,
            hobby
        };

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
        "Barista"
    };

    Variant const hobby2_var{Variant::Map{
        {"id", Variant(2)},
        {"description", Variant("Barista")}
    }};


    PersonEx const person_ex{
        "P",
        std::vector<Hobby>{hobby, hobby2}
    };

    Variant person_ex_var{Variant::Map{
        std::make_pair("name", Variant("P")),
        std::make_pair("hobbies", Variant(Variant::Vec{Variant(hobby_var), hobby2_var}))
    }};

    SECTION("Check struct with vector") {
        REQUIRE(person_ex == PersonEx::fromVariant(person_ex_var));
        REQUIRE(person_ex_var == PersonEx::toVariant(person_ex));
    }

    SECTION("Check dict") {
        Dict const expected{
            5,
            {{"1", 1}, {"2", 2}}
        };

        Variant const tmp{Variant::Map{
            {"x", Variant(5)},
            {"map", Variant({{"1", Variant(1)}, {"2", Variant(2)}})}
        }};

        REQUIRE(expected == Dict::fromVariant(tmp));
        REQUIRE(tmp == Dict::toVariant(expected));
    }
}


namespace {


struct PersonD
        : trait::VarDef<PersonD>
        , trait::OStream<PersonD>
        , trait::EqualityComparison<PersonD> {
    PersonD() {}

    PersonD(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    PersonD(std::string const& name, Hobby const& hobby)
        : name(name), hobby(hobby)
    {}

    static auto defaults() {
        return hana::make_map(
                    hana::make_pair("name"_s, "Efendi"s)
        );
    }

    std::string name;
    std::optional<int> age;
    Hobby hobby;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(PersonD, name, age, hobby);


TEST_CASE("Check trait::VarDef", "[variant_traits]") {
    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}
    };

    Variant::Map person_var{
        {"name", Variant("Alecu")},
        {"age", Variant(16)},
        {"hobby", Variant(hobby_var)}
    };

    Hobby const hobby{
        1,
        std::string("Hack")
    };

    PersonD person{
        "Alecu",
        16,
        hobby
    };

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
}


namespace {


struct PersonC
        : trait::VarDefExplicit<PersonC>
        , trait::OStream<PersonC>
        , trait::EqualityComparison<PersonC> {
    PersonC() {}

    PersonC(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    static auto defaults() {
        return hana::make_map(
                    hana::make_pair("name"_s, "Efendi"),
                    hana::make_pair("age"_s, 18),
                    hana::make_pair("hobby"_s, trait::NoDefault())
        );
    }

    std::optional<std::string> name;
    std::optional<int> age;
    Hobby hobby;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(PersonC, name, age, hobby);


TEST_CASE("Check trait::VarDefExplicit", "[variant_traits]") {
    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}
    };

    Variant::Map person_var{
        {"hobby", Variant(hobby_var)}
    };

    Hobby const hobby{
        1,
        std::string("Hack")
    };

    PersonC person{
        "Efendi",
        18,
        hobby
    };

    REQUIRE(PersonC::fromVariant(Variant(person_var)) == person);
}


namespace {


struct Car : trait::Var<Car> {
    Car(std::string const& name, int wheels) : name(name), wheels(wheels) {}
    Car() = default;

    std::string name;
    int wheels{0};
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Car, name, wheels);


TEST_CASE("Check trait::Var fails", "[variant_traits]") {
    Variant car_var{Variant::Map{
        {"name", Variant("saab")},
        {"wheels", Variant("4")}
    }};

    REQUIRE_THROWS_AS(Car::fromVariant(car_var), VariantBadType);
    REQUIRE(hana::equal(int(1), int(1)));
}
