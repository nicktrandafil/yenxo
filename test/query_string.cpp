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

#include <yenxo/query_string.hpp>
#include <yenxo/variant.hpp>

#include <catch2/catch.hpp>

#include <regex>

using namespace yenxo;

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
    SECTION("empty param") {
        REQUIRE(query_string("") == Variant::fromJson(R"({})"));
    }

    SECTION("two empty param") {
        REQUIRE(query_string("&") == Variant::fromJson(R"({})"));
    }

    SECTION("three empty param") {
        REQUIRE(query_string("&&") == Variant::fromJson(R"({})"));
    }

    SECTION("three empty params, an invalid param") {
        REQUIRE_THROWS_AS(query_string("&&x&"), QueryStringError);
        REQUIRE_THROWS_WITH(query_string("&&x&"), R"(expecting "=" here: "&")");
        try {
            (void)query_string("&&x&");
        } catch (QueryStringError const& e) {
            REQUIRE(e.isParseError());
            INFO(e.prettyParseError());
            REQUIRE(e.prettyParseError() == R"(Error! Expecting "=" here: "&&x&"
                            ___^ )");
        }
    }

    SECTION("three empty params, a key with equal") {
        REQUIRE(query_string("&&x=&") == Variant::fromJson(R"({"x": ""})"));
    }

    SECTION("three empty params, a value") {
        REQUIRE_THROWS_AS(query_string("&&=x&"), QueryStringError);
        REQUIRE_THROWS_WITH(
                query_string("&&=x&"),
                R"(expecting <alternative><parameter><empty_parameter> here: "=x&")");
    }

    SECTION("two empty params, an invalid key") {
        REQUIRE_THROWS_AS(query_string("&a[b]d=x&"_b), QueryStringError);
        REQUIRE_THROWS_WITH(query_string("&a[b]d=x&"_b), R"(expecting "=" here: "d=x&")");
    }

    SECTION("braces in value are allowed (use upper case in hex)") {
        REQUIRE(query_string("xy=%5bxy%5D") == Variant::fromJson(R"({"xy": "[xy]"})"));
    }

    SECTION("breaces in key depthen the object (use mixed case in hex") {
        REQUIRE(query_string("xy%5ba%5d=%5bxy%5D")
                == Variant::fromJson(R"({"xy": {"a": "[xy]"}})"));
    }

    SECTION("character after brackets") {
        REQUIRE_THROWS_AS(query_string("a[b]d=x"), QueryStringError);
        REQUIRE_THROWS_WITH(query_string("a[b]d=x"), R"(expecting "=" here: "d=x")");
    }

    SECTION("expected closing bracket") {
        REQUIRE_THROWS_WITH(query_string("a[b[=x"),
                            R"(expecting <close_bracket> here: "[=x")");
    }

    SECTION("expected opening bracket") {
        REQUIRE_THROWS_WITH(query_string("a]b=x"), R"(expecting "=" here: "]b=x")");
    }

    SECTION("invalid xdigit") {
        REQUIRE_THROWS_WITH(query_string("a%5x=x"), R"(expecting <xdigit> here: "x=x")");
    }

    SECTION("invalid xdigit") {
        REQUIRE_THROWS_WITH(query_string("a=="),
                            R"(expecting <alternative><value><empty_value> here: "=")");
    }

    SECTION("double close bracket") {
        REQUIRE_THROWS_WITH(query_string("a[b]]=x"), R"(expecting "=" here: "]=x")");
    }

    SECTION("brackets are allowed in value") {
        REQUIRE(query_string("a=x[0]") == R"({"a": "x[0]"})"_j);
    }

    SECTION("brackets are allowed in value") {
        REQUIRE(query_string("a=x%5b0%5d") == R"({"a": "x[0]"})"_j);
    }

    SECTION("simple value") {
        /// [simple_value]
        REQUIRE(query_string("a=b") == R"({"a": "b"})"_j);
        /// [simple_value]
    }

    SECTION("array by duplicate") {
        /// [array_by_duplicate]
        REQUIRE(query_string("a=b&a=c") == R"({"a": ["b", "c"]})"_j);
        /// [array_by_duplicate]
    }

    SECTION("array by empty square brackets") {
        /// [array_by_empty_brackets]
        REQUIRE(query_string("a[]=b") == R"({"a": ["b"]})"_j);
        /// [array_by_empty_brackets]
    }

    SECTION("array by empty square brackets two values") {
        REQUIRE(query_string("a[]=b&a[]=c") == R"({"a": ["b", "c"]})"_j);
    }

    SECTION("array by empty square brackets in map") {
        REQUIRE(query_string("a[b][]=b&a[b][]=c") == R"({"a": {"b": ["b", "c"]}})"_j);
    }

    SECTION("array by empty square are not allowed in the middle of a parameter") {
        REQUIRE_THROWS_WITH(query_string("a[][u]=b"), R"(expecting "=" here: "[u]=b")");
    }

    SECTION("array by index") {
        REQUIRE(query_string("a[0]=b&a[1]=c"_b) == R"({"a": ["b", "c"]})"_j);
    }

    SECTION("array by index, reverse") {
        REQUIRE(query_string("a[1]=b&a[0]=c"_b) == R"({"a": ["c", "b"]})"_j);
    }

    SECTION("array by index, fill till specified") {
        /// [array_by_index]
        REQUIRE(query_string("a[1]=b"_b) == R"({"a": [null, "b"]})"_j);
        /// [array_by_index]
    }

    SECTION("simple map") {
        /// [simple_object]
        REQUIRE(query_string("a[b]=c") == R"({"a": {"b": "c"}})"_j);
        /// [simple_object]
    }

    SECTION("simple map with percent encoded brackets") {
        /// [simple_object_enc]
        REQUIRE(query_string("a%5bb%5d=c") == R"({"a": {"b": "c"}})"_j);
        /// [simple_object_enc]
    }

    SECTION("mixed array") {
        REQUIRE(query_string("a=1&a[2]=2"_b) == R"({"a": ["1", null, "2"]})"_j);
    }

    SECTION("array item count almost limit exceed") {
        REQUIRE(query_string(
                        "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20"_b)
                == R"({"a": ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"]})"_j);
    }

    SECTION("array item count limit exceed") {
        REQUIRE_THROWS_AS(
                query_string(
                        "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20&a=21"_b),
                QueryStringError);
        REQUIRE_THROWS_WITH(
                query_string(
                        "a=1&a=2&a=3&a=4&a=5&a=6&a=7&a=8&a=9&a=10&a=11&a=12&a=13&a=14&a=15&a=16&a=17&a=18&a=19&a=20&a=21"_b),
                "array length exceed 20 for a");
    }

    SECTION("array in object") {
        /// [array_in_object]
        REQUIRE(query_string("a[b][1]=1&a[b]=2"_b)
                == R"({"a": {"b": [null, "1", "2"]}})"_j);
        /// [array_in_object]
    }

    SECTION("object in array") {
        /// [object_in_array]
        REQUIRE(query_string("a[1][b]=1&a[1][c]=2"_b)
                == R"({"a": [null, {"b": "1", "c": "2"}]})"_j);
        /// [object_in_array]
    }

    SECTION("object property count limit almost exceed") {
        REQUIRE_NOTHROW(query_string(
                "a[a]=1&a[b]=2&a[c]=3&a[d]=4&a[e]=5&a[f]=6&a[g]=7&a[h]=8&a[i]=9&a[j]=10&a[k]=11&a[l]=12&a[m]=13&a[n]=14&a[o]=15&a[p]=16&a[q]=17&a[r]=18&a[s]=19&a[t]=20"_b));
    }

    SECTION("object property count limit exceed for") {
        REQUIRE_THROWS_WITH(
                query_string(
                        "a[a]=1&a[b]=2&a[c]=3&a[d]=4&a[e]=5&a[f]=6&a[g]=7&a[h]=8&a[i]=9&a[j]=10&a[k]=11&a[l]=12&a[m]=13&a[n]=14&a[o]=15&a[p]=16&a[q]=17&a[r]=18&a[s]=19&a[t]=20&a[u]=21"_b),
                "object property count exceed 20 for a");
    }

    SECTION("object property count limit exceed") {
        REQUIRE_THROWS_WITH(
                query_string(
                        "a=1&b=2&c=3&d=4&e=5&f=6&g=7&h=8&i=9&j=10&k=11&l=12&m=13&n=14&o=15&p=16&q=17&r=18&s=19&t=20&u=21"_b),
                "object property count limit exceed 20");
    }

    SECTION("object depth limit exceed for") {
        REQUIRE_THROWS_WITH(
                query_string(
                        "a[b][c][d][e][f][g][h][i][j][k][l][m][n][o][p][q][r][s][t][u][w]=21"_b),
                "object depth limit exceed 20 for a");
    }

    SECTION("array length limit exceed for, big index") {
        REQUIRE_THROWS_WITH(query_string("a[20]=1"_b),
                            "array index out of range [0, 19] for a");
    }

    SECTION("mixed types for array") {
        REQUIRE_THROWS_WITH(query_string("a[a]=1&a[0]=1"_b),
                            "mixed types for a: vec and map");
    }

    SECTION("mixed types for map") {
        REQUIRE_THROWS_WITH(query_string("a[0]=1&a[a]=1"_b),
                            "mixed types for a: map and vec");
    }

    SECTION("mixed types for map") {
        REQUIRE_THROWS_WITH(query_string("a[a]=1&a=2"_b),
                            "mixed types for a: map and string");
    }

    SECTION("percent encoded 1") {
        REQUIRE(query_string("a[%61]=%6d"_b) == R"({"a": {"a": "m"}})"_j);
        REQUIRE(query_string("a[%61]=%6D"_b) == R"({"a": {"a": "m"}})"_j);
    }

    SECTION("mix empty brackets with duplicate params") {
        REQUIRE(query_string("a=1&a[]=2"_b) == R"({"a": ["1", "2"]})"_j);
    }

    SECTION("out of range index") {
        REQUIRE_THROWS_WITH(query_string("a[999999999999999]=1"_b),
                            "array index out of range [0, 19] for a");
    }

    SECTION("syntax error") {
        REQUIRE_THROWS_WITH(query_string("a[[x]=1"),
                            R"(expecting <close_bracket> here: "[x]=1")");
    }

    SECTION("mixed empty brackets with map") {
        REQUIRE_THROWS_WITH(query_string("a[x]=1&a[]=2"),
                            R"(mixed types for a: vec and map)");
    }
}
