#include <yenxo/variant.hpp>
#include <yenxo/variant_traits.hpp>

#include <rapidjson/document.h>

using namespace yenxo;

struct Hobby : trait::Var<Hobby> { // enables serialization/deserialization
    Hobby()
            : id(0) {
    }

    Hobby(int id, std::string const& description)
            : id(id)
            , description(description) {
    }

    int id;
    std::string description;
};

struct Person : trait::Var<Person> {
    Person()
            : age(0) {
    }

    Person(std::string const& name, int age, Hobby const& hobby)
            : name(name)
            , age(age)
            , hobby(hobby) {
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

    Person const p =
            Person::fromVariant(Variant::from(rapidjson::Document().Parse(json)));
}

// add reflection
BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
