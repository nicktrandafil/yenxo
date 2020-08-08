# serialize

[![Build Status](https://travis-ci.com/nicktrandafil/serialize.svg?branch=master)](https://travis-ci.com/nicktrandafil/serialize)
[![codecov](https://codecov.io/gh/nicktrandafil/serialize/branch/master/graph/badge.svg)](https://codecov.io/gh/nicktrandafil/serialize)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://nicktrandafil.github.io/serialize)

Some *add-in*'s which enable the traits listed below for user defined types:
* serialization/deserialization;
* comparison;
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

BOOST_HANA_ADAPT_STRUCT(Person, name, age, height);

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

More snippets can be found in `examples` and `test` directories.
The documentation can be found [here](https://nicktrandafil.github.io/serialize).
