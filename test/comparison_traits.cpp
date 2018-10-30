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
#include <rproject/comparison_traits.hpp>

// local
#include <rproject/ostream_traits.hpp>

// 3rd
#include <catch2/catch.hpp>


using namespace rp;


namespace {


struct Hobby
        : trait::EqualityComparison<Hobby>
        , trait::OStream<Hobby> {
    Hobby(int id, std::string const& description)
        : id(id), description(description)
    {}

    int id;
    std::string description;
};


struct Person
        : trait::EqualityComparison<Person>
        , trait::OStream<Person> {
    Person(std::string const& name, int age, Hobby const& hobby)
        : name(name), age(age), hobby(hobby)
    {}

    std::string name;
    std::optional<int> age;
    Hobby hobby;
};


} // namespace


BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);


TEST_CASE("Check trait::EqualityComparison", "[comparison_traits]") {
    Hobby const hobby{
        1,
        std::string("Hack")
    };

    Person person{
        "Efendi",
        18,
        hobby
    };

    Person person2(person);
    REQUIRE(person == person2);
    REQUIRE_FALSE(person != person2);

    (*person2.age)++;
    REQUIRE(person != person2);
}
