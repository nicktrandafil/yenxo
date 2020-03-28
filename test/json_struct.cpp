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

#include <serialize/comparison_traits.hpp>
#include <serialize/ostream_traits.hpp>
#include <serialize/variant.hpp>
#include <serialize/variant_traits.hpp>

#include <catch2/catch.hpp>

#include <rapidjson/document.h>

using namespace serialize;

namespace {

struct Hobby : trait::Var<Hobby>,
               trait::UpdateFromVar<Hobby>,
               trait::EqualityComparison<Hobby> {

    Hobby() : id(0) {
    }

    Hobby(int id, std::string const& description) : id(id), description(description) {
    }

    int id;
    std::string description;
};

struct Person : trait::VarDef<Person>,
                trait::UpdateFromVar<Person>,
                trait::EqualityComparison<Person> {

    Person() : age(0) {
    }

    Person(std::string const& name, int age, Hobby const& hobby, bool b, unsigned u,
           long l, unsigned long ul, double f, std::vector<int>&& v)
        : name(name), age(age), hobby(hobby), b(b), u(u), l(l), ul(ul), f(f),
          v(std::move(v)) {
    }

    std::string name;
    int age;
    Hobby hobby;

    bool b;
    unsigned u;
    long l;
    unsigned long ul;
    double f;

    std::vector<int> v;

    std::optional<bool> n;
};

} // namespace

BOOST_HANA_ADAPT_STRUCT(Hobby, id, description);
BOOST_HANA_ADAPT_STRUCT(Person, name, age, hobby, b, u, l, ul, f, v, n);

TEST_CASE("Check simple json to struct", "[json_struct]") {
    auto const json = R"(
        {
            "name": "Efendi",
            "age": 20,
            "hobby": {
                "id": 10,
                "description": "Barista"
            },

            "b": true,
            "u": 4294967295,
            "l": 9223372036854775807,
            "ul": 18446744073709551615,
            "f": 1.1,
            "v": [1, 2]
        }
    )";

    Hobby const hobby{10, "Barista"};

    Person const expected{"Efendi",
                          20,
                          hobby,
                          true,
                          4294967295u,
                          9223372036854775807,
                          18446744073709551615U,
                          1.1,
                          {1, 2}};

    rapidjson::Document d;
    d.Parse(json);

    REQUIRE(expected == Person::fromVariant(Variant::from(d)));

    rapidjson::Document d2;
    REQUIRE(d == Person::toVariant(expected).to(d2));
}
