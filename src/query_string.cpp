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
#include <serialize/string_conversion.hpp>
#include <serialize/variant.hpp>

#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace fus = boost::fusion;
namespace ascii = boost::spirit::qi::ascii;

namespace serialize {
namespace {
int8_t digit(char digit) {
    if (digit <= '9') {
        assert(digit >= '0');
        return digit - '0';
    } else if (digit <= 'F') {
        assert(digit >= 'A');
        return 10 + digit - 'A';
    }
    assert(digit <= 'f');
    assert(digit >= 'a');
    return 10 + digit - 'a';
}

int8_t byte(char digit1, char digit2) {
    return 16 * digit(digit1) + digit(digit2);
}

// clang-format off
BOOST_FUSION_DEFINE_STRUCT_INLINE(Key,
    (std::string, name)
    (std::vector<std::string>, nested_keys)
)

BOOST_FUSION_DEFINE_STRUCT_INLINE(Parameter,
    (Key, key)
    (std::string, value)
)
// clang-format on

template <class Iterator>
class Grammar : public qi::grammar<Iterator, std::vector<Parameter>()> {
public:
    Grammar() : Grammar::base_type(query_string) {
        using phx::at_c;
        using phx::construct;
        using phx::val;
        using qi::alnum;
        using qi::char_;
        using qi::eoi;
        using qi::eps;
        using qi::expect;
        using qi::fail;
        using qi::lexeme;
        using qi::lit;
        using qi::on_error;
        using qi::xdigit;

        using namespace qi::labels;

        unreserved %= alnum | char_('-') | char_(".") | char_("_") | char_("~");

        encoded_open_bracket %= lit("%5b") | lit("%5B");
        encoded_close_bracket %= lit("%5d") | lit("%5D");

        pct_encoded = lexeme[lit('%') > xdigit > xdigit]
                            [_val = phx::bind(&byte, at_c<0>(_1), at_c<1>(_1))] -
                      (eps(_r1) >> (encoded_open_bracket | encoded_close_bracket));

        sub_delims %= char_('!') | char_('$') | char_('\'') | char_('(') | char_(')') |
                      char_('*') | char_('+') | char_(',') | char_(';');

        pchar %= unreserved | pct_encoded(_r1) | sub_delims | char_(':') | char_('@');

        key %= +pchar(val(true)) >>
               *(encoded_open_bracket > *pchar(val(true)) > encoded_close_bracket);

        value %= *pchar(val(false));

        parameter %= key > lit('=') > value;

        empty_parameter %= &lit('&') | eoi;

        query_string %= expect[parameter | empty_parameter] % lit('&');

        unreserved.name("unreserved");
        pct_encoded.name("pct_encoded");
        pchar.name("pchar");
        sub_delims.name("sub_delims");
        encoded_open_bracket.name("encoded_open_bracket");
        encoded_close_bracket.name("encoded_close_bracket");
        key.name("key");
        value.name("value");
        parameter.name("parameter");
        empty_parameter.name("empty_parameter");
        query_string.name("query_string");

        on_error<fail>(query_string,
                       phx::bind(&Grammar::saveError, phx::ref(*this), _4, _1, _3, _2));
    }

    std::string const& errorExpectation() const noexcept {
        return error_expectation;
    }

    size_t errorPos() const noexcept {
        return error_pos;
    }

private:
    void saveError(boost::spirit::info const& info, Iterator begin, Iterator error_pos,
                   Iterator) {
        std::stringstream s;
        s << info;
        error_expectation = s.str();
        this->error_pos = static_cast<size_t>(std::distance(begin, error_pos));
    }

private:
    qi::rule<Iterator, char()> unreserved;
    qi::rule<Iterator, char(bool)> pct_encoded;
    qi::rule<Iterator, char(bool)> pchar;
    qi::rule<Iterator, char()> sub_delims;
    qi::rule<Iterator, void()> encoded_open_bracket;
    qi::rule<Iterator, void()> encoded_close_bracket;
    qi::rule<Iterator, Key()> key;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, Parameter()> parameter;
    qi::rule<Iterator, void()> empty_parameter;
    qi::rule<Iterator, std::vector<Parameter>()> query_string;

    std::string error_expectation;
    size_t error_pos;
};

std::string make_key(std::string name, std::vector<std::string>::const_iterator b,
                     std::vector<std::string>::const_iterator e) {
    for (auto it = b; it != e; ++it) { name += "[" + *it + "]"; }
    return name;
}

} // namespace

std::string QueryStringError::prettyParseError() const {
    std::string line{input};
    std::string highlight(input.size(), '_');

    highlight[error_pos] = '^';

    line = "\"" + line + "\"";
    highlight = " " + highlight + " ";

    std::string const prefix = "Error! Expecting " + expected + " here: ";
    line = prefix + line;
    highlight = std::string(prefix.size(), ' ') + highlight;

    return line + "\n" + highlight;
}

namespace {

std::vector<Parameter> parseParameters(std::string const& str) {
    std::vector<Parameter> out;
    Grammar<std::string::const_iterator> grammar;
    auto const res = qi::parse(str.begin(), str.end(), grammar, out);
    if (!res) {
        throw QueryStringError("expecting " + grammar.errorExpectation() + " here: \"" +
                                       str.substr(grammar.errorPos()) + "\"",
                               str, grammar.errorExpectation(), grammar.errorPos());
    }
    return out;
}

} // namespace

void query_string(Variant& out, std::string const& str) {
    // limits
    constexpr auto const array_length_limit = 20;
    constexpr auto const object_depth_limit = 20;
    constexpr auto const object_property_count_limit = 20;

    // parse
    auto const parameters = parseParameters(str);

    // construct DOM
    for (auto const& p : parameters) {
        auto param = &out.modifyMap()[p.key.name];

        if (out.map().size() > object_property_count_limit) {
            throw QueryStringError("object property count limit exceed " +
                                   std::to_string(object_property_count_limit));
        }

        if (p.key.nested_keys.size() > object_depth_limit) {
            throw QueryStringError("object depth limit exceed " +
                                   std::to_string(object_depth_limit) + " for " +
                                   p.key.name);
        }

        for (auto key_it = p.key.nested_keys.begin(); key_it != p.key.nested_keys.end();
             ++key_it) {
            auto const& key_str = *key_it;

            int64_t array_index{-1};
            try {
                auto const tmp = std::stol(key_str);
                if (tmp >= 0) { array_index = static_cast<int8_t>(tmp); }
            } catch (std::exception const&) {}

            if (array_index != -1) { // array
                if (array_index > array_length_limit) {
                    auto const ptr =
                            make_key(p.key.name, p.key.nested_keys.begin(), key_it);
                    throw QueryStringError("array length limit exceed " +
                                           std::to_string(array_length_limit) + " for " +
                                           ptr);
                }

                if (param->type() == Variant::TypeTag::null) {
                    *param = Variant(VariantVec());
                } else if (param->type() == Variant::TypeTag::map) {
                    throw QueryStringError(
                            "mixed types for " +
                            make_key(p.key.name, p.key.nested_keys.begin(), key_it) +
                            ": vec and " + toString(param->type()));
                } else if (param->type() != Variant::TypeTag::vec) {
                    *param = Variant(VariantVec{*param});
                }
                auto& vec = param->modifyVec();
                while (static_cast<size_t>(array_index) >= vec.size()) {
                    vec.push_back(Variant());
                }
                param = &vec[static_cast<size_t>(array_index)];
            } else { // object
                if (param->type() == Variant::TypeTag::null) {
                    *param = Variant(VariantMap());
                } else if (param->type() != Variant::TypeTag::map) {
                    throw QueryStringError(
                            "mixed types for " +
                            make_key(p.key.name, p.key.nested_keys.begin(), key_it) +
                            ": map and " + toString(param->type()));
                }
                auto& map = param->modifyMap();
                param = &map[key_str];
                if (map.size() > object_property_count_limit) {
                    auto const ptr =
                            make_key(p.key.name, p.key.nested_keys.begin(), key_it);
                    throw QueryStringError("object property count limit exceed " +
                                           std::to_string(object_property_count_limit) +
                                           " for " + ptr);
                }
            }
        }

        if (param->null()) {
            *param = Variant(p.value);
        } else if (param->type() == Variant::TypeTag::vec) {
            param->modifyVec().push_back(Variant(p.value));
            if (param->vec().size() > array_length_limit) {
                throw QueryStringError("array length limit exceed " +
                                       std::to_string(array_length_limit) + " for " +
                                       make_key(p.key.name, p.key.nested_keys.begin(),
                                                p.key.nested_keys.end()));
            }
        } else if (param->type() == Variant::TypeTag::map) {
            throw std::runtime_error("mixed types for " +
                                     make_key(p.key.name, p.key.nested_keys.begin(),
                                              p.key.nested_keys.end()) +
                                     ": map and string");
        } else {
            *param = Variant(VariantVec{*param, Variant(p.value)});
        }
    }
}

} // namespace serialize
