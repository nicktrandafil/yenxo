#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL

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


std::ostream& operator<<(std::ostream& os, Hobby const& hobby) {
    os << "Hobby:\n"
       << "  id: " << hobby.id << '\n'
       << "  description: " << hobby.description << '\n';
    return os;
}


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


TEST_CASE("Check Var and UpdateFromVar", "[variant_mixins]") {
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

    SECTION("Check no field") {
        person_var.erase("name");
        REQUIRE_THROWS_AS(
                    Person::fromVariant(Variant(person_var)),
                    std::logic_error);
    }
}

struct PersonD : mixin::VarDef<PersonD> {
    PersonD() {}

    PersonD(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    PersonD(std::string const& name, Hobby const& hobby)
        : name(name), hobby(hobby)
    {}


    constexpr static auto defaultMemVals() {
        using namespace hana::literals;

        return hana::make_map(
            hana::make_pair("name"_s, "Efendi")
        );
    }

    BOOST_HANA_DEFINE_STRUCT(
            PersonD,
            (std::string, name),
            (std::optional<int>, age),
            (Hobby, hobby)
    );
};


std::ostream& operator<<(std::ostream& os, PersonD const& person) {
    os << "Person:\n"
       << "  name: " << person.name << '\n';

    if (person.age) {
        os << "  age: " << *person.age << '\n';
    } else {
        os << "  age: " << "none\n";
    }

    os << person.hobby;

    return os;
}


TEST_CASE("Check mixin::VarDef", "[variant_mixins]") {
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
        INFO(actual);
        REQUIRE(hana::equal(actual, PersonD{"Efendi", 16, hobby}));
    }

    SECTION("Check fromVariant with no age (no default value, but type is optional)") {
        person_var.erase("age");
        auto const actual = PersonD::fromVariant(Variant(person_var));
        REQUIRE(hana::equal(actual, PersonD{"Alecu", hobby}));
    }

    SECTION("Check fromVariant with no hobby (no default value)") {
        person_var.erase("hobby");
        REQUIRE_THROWS_AS(PersonD::fromVariant(Variant(person_var)), std::logic_error);
    }
}
