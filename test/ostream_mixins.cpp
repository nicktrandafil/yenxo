// tested
#include <ostream_mixins.hpp>

// 3rd
#include <catch2/catch.hpp>


namespace {


struct Hobby
        : mixin::OStream<Hobby> {
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
        : mixin::OStream<Person> {

    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    BOOST_HANA_DEFINE_STRUCT(
            Person,
            (std::string, name),
            (std::optional<int>, age),
            (Hobby, hobby)
    );
};


} // namespace


TEST_CASE("Check mixin::OStream", "[ostream_mixins]") {
    Hobby const hobby{
        1,
        std::string("Hack")
    };

    Person person{
        "Efendi",
        18,
        hobby
    };

    std::ostringstream os;
    os << person;

    REQUIRE(os.str() == "Person { name: Efendi; age: 18; hobby: Hobby { id: 1; description: Hack; }; }");
}
