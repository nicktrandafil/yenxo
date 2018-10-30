# rproject

[![Build Status](https://travis-ci.com/nicktrandafil/rproject.svg?branch=master)](https://travis-ci.com/nicktrandafil/rproject)
[![codecov](https://codecov.io/gh/nicktrandafil/rproject/branch/master/graph/badge.svg)](https://codecov.io/gh/nicktrandafil/rproject)

Some *add-in*'s which enables the traits listed below for user defined types:
* serialization/deserialization in application boundaries;
* comparation;
* pushing to `std::ostream`.

Uses `boost::hana` in order to obtain reflection in C++.
Uses `rapidjson` in order to parse and serialize JSON.

## Examples

Typical use case of `trait::Var`:

```cpp
struct Person : trait::Var<Person> {
    std::string name;
    int age;
    float height;
};

BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);

auto const json = R"(
    {
        "name": "An uncommon name",
        "age": 20,
        "height": "170.5"
    }
)";

// Construct from JSON
Person person(Variant::from(rapidjson::Document().Parse(json)));
```

For more examples see `examples` dir and tests.
