// tested
#include <variant_mixins.hpp>

// local
#include <variant.hpp>

// 3rd
#include <catch.hpp>

// boost
#include <boost/hana.hpp>


namespace hana = boost::hana;


struct Hobby
        : mixin::Var<Hobby>
        , mixin::UpdateFromVar<Hobby> {

    Hobby() : id(0) {}

    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    BOOST_HANA_DEFINE_STRUCT(
            Hobby,
            (int, id),
            (std::string, description)
    );
};


struct Person
        : mixin::Var<Person>
        , mixin::UpdateFromVar<Person> {

    Person() : age(0) {}

    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    BOOST_HANA_DEFINE_STRUCT(
            Person,
            (std::string, name),
            (int, age),
            (Hobby, hobby)
    );
};


TEST_CASE("detail::toVariant", "[variant_mixin_helpers]") {
    auto const x = detail::toVariant(5);
    REQUIRE(x == Variant(5));
    REQUIRE(5 == detail::fromVariant<int>(Variant(5)));

    auto const x1 = detail::toVariant("Hello");
    REQUIRE(x1 == Variant("Hello"));
    REQUIRE("Hello" == detail::fromVariant<std::string>(Variant("Hello")));

    struct X {
        static Variant toVariant(X const& x) { return Variant(x.m); }
        static X fromVariant(Variant const& x) { return {x.integer()}; }
        int m{5};
    };

    auto const x2 = detail::toVariant(X());
    REQUIRE(x2 == Variant(5));
    REQUIRE(6 == detail::fromVariant<X>(Variant(6)).m);
}


TEST_CASE("Check mixins", "[variant_mixins]") {
    Variant::Map const hobby_var{
        {"id", Variant(1)},
        {"description", Variant("Hack")}
    };

    Variant::Map const person_var{
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
        REQUIRE(hana::equal(person, Person::fromVariant(Variant(person_var))));
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

        REQUIRE(hana::equal(person_updated, person));
    }
}
