# rproject

[![Build Status](https://travis-ci.com/nicktrandafil/rproject.svg?branch=master)](https://travis-ci.com/nicktrandafil/rproject)

Some *add-in*'s which enables the traits listed below for user defined types:
* serialization/deserialization in application boundaries;
* comparation;
* pushing to `std::ostream`.

Uses `boost::hana` in order to obtain reflection in C++.
Uses `rapidjson` in order to parse and serialize JSON.

## Examples

Typical use case of `traits::Var`:

```cpp
// rproject
#include <variant.hpp>
#include <variant_traits.hpp>

struct Hobby : trait::Var<Hobby> {  // enables serialization/deserialization
    Hobby() : id(0) {}

    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    int id;
    std::string description;
};

struct Person : trait::Var<Person> {
    Person() : age(0) {}

    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

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

    Person const p = Person::fromVariant(Variant::from(rapidjson::Document().Parse(json))));
}

// add reflection
BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
```

Typical use case of `trait::OStream`, `EqualityComparison`:

```cpp
// rapidjson
#include <variant.hpp>
#include <variant_traits.hpp>
#include <ostream_traits.hpp>
#include <comparison_traits.hpp>

// std
#include <iostream>

struct Hobby
        : trait::Var<Hobby>
        , trait::OStream<Hobby>              // enables `std::ostream`
        , trait::EqualityComparison<Hobby> { // enables `==` and `!=`
    Hobby() : id(0) {}

    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    int id;
    std::string description;
};

struct Person
        : trait::Var<Person>
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

    Hobby const hobby{
        10,
        "Barista"
    };

    Person const expected{
        "Efendi",
        20,
        hobby
    };

    auto const person = Person::fromVariant(Variant::from(rapidjson::Document().Parse(json)));
    assert(person == expected);
    std::cout << person;
}

BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
```
