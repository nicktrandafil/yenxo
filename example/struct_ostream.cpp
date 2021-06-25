#include <yenxo/comparison_traits.hpp>
#include <yenxo/ostream_traits.hpp>
#include <yenxo/variant.hpp>
#include <yenxo/variant_traits.hpp>

#include <rapidjson/document.h>

#include <iostream>

using namespace yenxo;

struct Hobby
    : trait::Var<Hobby>,
      trait::OStream<Hobby> // enables `std::ostream`
    ,
      trait::EqualityComparison<Hobby> { // enables `==` and `!=`
    Hobby()
            : id(0) {
    }

    Hobby(int id, std::string const& description)
            : id(id)
            , description(description) {
    }

    friend constexpr std::string_view typeNameImpl(Hobby const*) noexcept {
        return "Hobby";
    }

    int id;
    std::string description;
};

struct Person
        : trait::Var<Person>
        , trait::OStream<Person>
        , trait::EqualityComparison<Person> {
    Person()
            : age(0) {
    }

    Person(std::string const& name, int age, Hobby const& hobby)
            : name(name)
            , age(age)
            , hobby(hobby) {
    }

    friend constexpr std::string_view typeNameImpl(Person const*) noexcept {
        return "Person";
    }

    std::string name;
    int age;
    Hobby hobby;
};

int main() {
    auto const json = R"(
        {
            "name": "Efendi",
            "age": 20,
            "hobby": {
                "id": 10,
                "description": "Barista"
            }
        }
    )";

    Hobby const hobby{10, "Barista"};

    Person const expected{"Efendi", 20, hobby};

    auto const person =
            Person::fromVariant(Variant::from(rapidjson::Document().Parse(json)));
    assert(person == expected);
    std::cout << person;
}

BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
