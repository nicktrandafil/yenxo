/*
  MIT License

  Copyright (c) 2020 Nicolai Trandafil

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

#include <serialize/query_string.hpp>
#include <serialize/variant.hpp>

#include <catch2/catch.hpp>

#include <regex>

using namespace serialize;

std::string operator""_b(char const* str, size_t s) {
    std::string ret(str, s);
    ret = std::regex_replace(ret, std::regex("\\["), "%5b");
    ret = std::regex_replace(ret, std::regex("\\]"), "%5d");
    return ret;
}

Variant operator""_j(char const* str, size_t s) {
    return Variant::fromJson(std::string(str, s));
}

TEST_CASE("Check query_string", "[query]") {
    serialize::Variant query = Variant(VariantMap());

    SECTION("empty param") {
        query_string(query, "");
        REQUIRE(query == Variant::fromJson(R"({"":""})"));
    }

    SECTION("two empty param") {
        query_string(query, "&");
        REQUIRE(query == Variant::fromJson(R"({"": ["", ""]})"));
    }

    SECTION("three empty param") {
        query_string(query, "&&");
        REQUIRE(query == Variant::fromJson(R"({"": ["", "", ""]})"));
    }

    SECTION("three empty params, an invalid param") {
        REQUIRE_THROWS_AS(query_string(query, "&&x&"), QueryStringError);
        REQUIRE_THROWS_WITH(query_string(query, "&&x&"), R"(expecting "=" here: "&")");
        try {
            query_string(query, "&&x&");
        } catch (QueryStringError const& e) {
            REQUIRE(e.isParseError());
            INFO(e.prettyParseError());
            REQUIRE(e.prettyParseError() == R"(Error! Expecting "=" here: "&&x&"
                            ___^ )");
        }
    }

    SECTION("three empty params, a key with equal") {
        query_string(query, "&&x=&");
        REQUIRE(query == Variant::fromJson(R"({"x": "", "": ["", "", ""]})"));
    }

    SECTION("three empty params, a value") {
        REQUIRE_THROWS_AS(query_string(query, "&&=x&"), QueryStringError);
        REQUIRE_THROWS_WITH(
                query_string(query, "&&=x&"),
                R"(expecting <alternative><parameter><empty_parameter> here: "=x&")");
    }

    SECTION("two empty params, an invalid key (brakets are not allowed)") {
        REQUIRE_THROWS_AS(query_string(query, "&a[b]d=x&"), QueryStringError);
        REQUIRE_THROWS_WITH(query_string(query, "&a[b]d=x&"),
                            R"(expecting "=" here: "[b]d=x&")");
    }

    SECTION("two empty params, an invalid key") {
        REQUIRE_THROWS_AS(query_string(query, "&a[b]d=x&"_b), QueryStringError);
        REQUIRE_THROWS_WITH(query_string(query, "&a[b]d=x&"_b),
                            R"(expecting "=" here: "d=x&")");
    }

    SECTION("braces in value are allowed (use upper case in hex)") {
        query_string(query, "xy=%5bxy%5D");
        REQUIRE(query == Variant::fromJson(R"({"xy": "[xy]"})"));
    }

    SECTION("breaces in key depthen the object (use mixed case in hex") {
        query_string(query, "xy%5ba%5d=%5bxy%5D");
        REQUIRE(query == Variant::fromJson(R"({"xy": {"a": "[xy]"}})"));
    }

    SECTION("array by duplicate") {
        query_string(query, "a=b&a=c");
        REQUIRE(query == R"({"a": ["b", "c"]})"_j);
    }

    SECTION("array by index") {
        query_string(query, "a[0]=b&a[1]=c"_b);
        REQUIRE(query == R"({"a": ["b", "c"]})"_j);
    }

    SECTION("array by index, reverse") {
        query_string(query, "a[1]=b&a[0]=c"_b);
        REQUIRE(query == R"({"a": ["c", "b"]})"_j);
    }

    SECTION("array by index, fill till specified") {
        query_string(query, "a[1]=b"_b);
        REQUIRE(query == R"({"a": [null, "b"]})"_j);
    }

    SECTION("mixed array") {
        query_string(query, "a=1&a[2]=2"_b);
        REQUIRE(query == R"({"a": ["1", null, "2"]})"_j);
    }

    SECTION("array item count almost limit exceed") {
        query_string(
                query,
                "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20"_b);
        REQUIRE(query ==
                R"({"a": ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"]})"_j);
    }

    SECTION("array item count limit exceed") {
        REQUIRE_THROWS_AS(
                query_string(
                        query,
                        "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20&a=21"_b),
                QueryStringError);
        query = Variant(VariantMap());
        REQUIRE_THROWS_WITH(
                query_string(
                        query,
                        "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20&a=21"_b),
                "array length limit exceed 20 for a");
    }

    SECTION("array in object") {
        query_string(query, "a[b][1]=1&a[b]=2"_b);
        REQUIRE(query == R"({"a": {"b": [null, "1", "2"]}})"_j);
    }

    SECTION("object in array") {
        query_string(query, "a[1][b]=1&a[1][c]=2"_b);
        REQUIRE(query == R"({"a": [null, {"b": "1", "c": "2"}]})"_j);
    }

    SECTION("object property count limit almost exceed") {
        REQUIRE_NOTHROW(query_string(
                query,
                "a[a]=1&a[b]=2&a[c]=3&a[d]=4&a[e]=5&a[f]=6&a[g]=7&a[h]=8&a[i]=9&a[j]=10&a[k]=11&a[l]=12&a[m]=13&a[n]=14&a[o]=15&a[p]=16&a[q]=17&a[r]=18&a[s]=19&a[t]=20"_b));
    }

    SECTION("object property count limit exceed for") {
        REQUIRE_THROWS_WITH(
                query_string(
                        query,
                        "a[a]=1&a[b]=2&a[c]=3&a[d]=4&a[e]=5&a[f]=6&a[g]=7&a[h]=8&a[i]=9&a[j]=10&a[k]=11&a[l]=12&a[m]=13&a[n]=14&a[o]=15&a[p]=16&a[q]=17&a[r]=18&a[s]=19&a[t]=20&a[u]=21"_b),
                "object property count limit exceed 20 for a");
    }

    SECTION("object property count limit exceed") {
        REQUIRE_THROWS_WITH(
                query_string(
                        query,
                        "a=1&b=2&c=3&d=4&e=5&f=6&g=7&h=8&i=9&j=10&k=11&l=12&m=13&n=14&o=15&p=16&q=17&r=18&s=19&t=20&u=21"_b),
                "object property count limit exceed 20");
    }

    SECTION("object depth limit exceed for") {
        REQUIRE_THROWS_WITH(
                query_string(
                        query,
                        "a[b][c][d][e][f][g][h][i][j][k][l][m][n][o][p][q][r][s][t][u][w]=21"_b),
                "object depth limit exceed 20 for a");
    }

    SECTION("array length limit exceed for, big index") {
        REQUIRE_THROWS_WITH(query_string(query, "a[21]=1"_b),
                            "array length limit exceed 20 for a");
    }

    SECTION("mixed types for array") {
        REQUIRE_THROWS_WITH(query_string(query, "a[a]=1&a[0]=1"_b),
                            "mixed types for a: vec and map");
    }

    SECTION("mixed types for map") {
        REQUIRE_THROWS_WITH(query_string(query, "a[0]=1&a[a]=1"_b),
                            "mixed types for a: map and vec");
    }

    SECTION("mixed types for map") {
        REQUIRE_THROWS_WITH(query_string(query, "a[a]=1&a=2"_b),
                            "mixed types for a: map and string");
    }
}
