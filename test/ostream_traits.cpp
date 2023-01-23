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
#include <yenxo/ostream_traits.hpp>

// 3rd
#include <catch2/catch_all.hpp>

using namespace yenxo;

namespace {

struct Hobby : trait::OStream<Hobby> {
    Hobby(int id, std::string const& description)
            : id(id)
            , description(description) {
    }

    friend constexpr std::string_view typeNameImpl(Hobby const*) {
        return "Hobby";
    }

    int id;
    std::string description;
};

struct Person : trait::OStream<Person> {

    Person(std::string const& name, int age, Hobby const& hobby)
            : name(name)
            , age(age)
            , hobby(hobby) {
    }

    friend constexpr std::string_view typeNameImpl(Person const*) {
        return "Person";
    }

    std::string name;
    std::optional<int> age;
    Hobby hobby;
};

struct Dict : trait::OStream<Dict> {
    Dict(int x, std::unordered_map<std::string, int> const& map)
            : x(x)
            , map(map) {
    }

    friend constexpr std::string_view typeNameImpl(Dict const*) {
        return "Dict";
    }

    int x;
    std::unordered_map<std::string, int> map;
};

struct Vec : trait::OStream<Vec> {
    Vec(int x, std::vector<int> const& vec)
            : x(x)
            , vec(vec) {
    }

    friend constexpr std::string_view typeNameImpl(Vec const*) {
        return "Vec";
    }

    int x;
    std::vector<int> vec;
};

} // namespace

BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby);
BOOST_HANA_ADAPT_STRUCT(Dict, x, map);
BOOST_HANA_ADAPT_STRUCT(Vec, x, vec);

TEST_CASE("Check trait::OStream", "[ostream_traits]") {
    Hobby const hobby{1, std::string("Hack")};

    Person person{"Efendi", 18, hobby};

    std::ostringstream os;

    SECTION("simple") {
        os << person;
        REQUIRE(os.str() == "Person { name: Efendi; age: 18; hobby: Hobby { id: 1; "
                            "description: Hack; }; }");
    }

    Dict const expected{1, {{"1", 1}, {"2", 2}}};

    SECTION("dict") {
        os << expected;
        REQUIRE(os.str() == "Dict { x: 1; map: { 2: 2; 1: 1; }; }");
    }

    SECTION("vec") {
        Vec const expected{1, {1, 2}};
        os << expected;
        REQUIRE(os.str() == "Vec { x: 1; vec: [1, 2]; }");
    }
}
