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
#include <yenxo/string_conversion.hpp>
#include <yenxo/variant.hpp>

#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

using namespace std::string_literals;

namespace yenxo {
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

QueryStringError makeObjectPropertyCountError(uint8_t len) {
    return QueryStringError("object property count limit exceed " + std::to_string(len));
}

QueryStringError makeObjectPropertyCountError(std::string const& key, uint8_t len) {
    return QueryStringError("object property count exceed " + std::to_string(len)
                            + " for " + key);
}

QueryStringError makeMixedTypesError(std::string const& key,
                                     std::string const& expected_type,
                                     std::string const& actual_type) {
    return QueryStringError("mixed types for " + key + ": " + expected_type + " and "
                            + actual_type);
}

QueryStringError makeArrayIndexError(std::string const& key, uint8_t len) {
    return QueryStringError("array index out of range [0, " + std::to_string(len - 1)
                            + "] for " + key);
}

QueryStringError makeArrayLengthError(std::string const& key, uint8_t len) {
    return QueryStringError("array length exceed " + std::to_string(len) + " for " + key);
}

QueryStringError makeObjectDepthError(std::string const& key, uint8_t len) {
    return QueryStringError("object depth limit exceed " + std::to_string(len) + " for "
                            + key);
}

template <class Iterator>
class Grammar : public qi::grammar<Iterator> {
public:
    explicit Grammar(QueryStringParseSettings const& parse_settings)
            : Grammar::base_type(query_string)
            , parse_settings(parse_settings) {
        using phx::at_c;
        using qi::alnum;
        using qi::char_;
        using qi::eoi;
        using qi::expect;
        using qi::fail;
        using qi::lexeme;
        using qi::lit;
        using qi::matches;
        using qi::omit;
        using qi::on_error;
        using qi::ulong_long;
        using qi::xdigit;

        using namespace qi::labels;

        open_bracket %= lit("%5b")[_val = '['] | lit("%5B")[_val = '['] | char_("[");
        close_bracket %= lit("%5d")[_val = ']'] | lit("%5D")[_val = ']'] | char_("]");

        pct_encoded = lexeme[lit('%') > xdigit > xdigit]
                            [_val = phx::bind(&byte, at_c<0>(_1), at_c<1>(_1))]
                    - (open_bracket | close_bracket);

        sub_delims %= char_('!') | char_('$') | char_('\'') | char_('(') | char_(')')
                    | char_('*') | char_('+') | char_(',') | char_(';');

        plus_as_space %= lit('+')[_val = ' '];

        unreserved %= alnum | char_('-') | char_(".") | char_("_") | char_("~");
        pchar %= unreserved | pct_encoded | sub_delims | char_(':') | char_('@');

        index %= omit[open_bracket] >> ulong_long > omit[close_bracket];
        property %= omit[open_bracket] >> +pchar > omit[close_bracket];
        empty_index = open_bracket > close_bracket;
        name %= +(plus_as_space | pchar);

        key = name[phx::bind(&Grammar::paramName, this, _1)]
            > *(index[phx::bind(&Grammar::indexOp, this, _1)]
                | property[phx::bind(&Grammar::propertyOp, this, _1)])
            > -empty_index[phx::bind(&Grammar::emptyIndexOp, this)];

        empty_value = &lit('&') | eoi;
        value %= +(plus_as_space | pchar | open_bracket | close_bracket);

        empty_parameter = &lit('&') | eoi;
        parameter = key > lit('=') > (value[phx::bind(&Grammar::val, this, _1)]
                                      | empty_value[phx::bind(&Grammar::emptyVal, this)]);

        query_string = expect[parameter | empty_parameter] % lit('&');

        unreserved.name("unreserved");
        pct_encoded.name("pct_encoded");
        pchar.name("pchar");
        sub_delims.name("sub_delims");
        open_bracket.name("open_bracket");
        close_bracket.name("close_bracket");
        property.name("property_operator");
        index.name("index_operator");
        empty_index.name("empty_index_operator");
        key.name("key");
        value.name("value");
        empty_value.name("empty_value");
        parameter.name("parameter");
        empty_parameter.name("empty_parameter");
        query_string.name("query_string");
        name.name("name");

        on_error<fail>(query_string, phx::bind(&Grammar::saveError, this, _4, _1, _3));
    }

    std::string const& errorExpectation() const noexcept {
        return error_expectation;
    }

    size_t errorExpectationPos() const noexcept {
        return error_expectation_pos;
    }

    VariantMap const& result() const noexcept {
        return out;
    }

private:
    void saveError(boost::spirit::info const& info, Iterator begin, Iterator error_pos) {
        std::stringstream s;
        s << info;
        error_expectation = s.str();
        this->error_expectation_pos =
                static_cast<size_t>(std::distance(begin, error_pos));
    }

    void incDepth() {
        ++depth;
        if (depth > parse_settings.object_depth_limit) {
            throw makeObjectDepthError(this->param_name,
                                       parse_settings.object_depth_limit);
        }
    }

    void paramName(std::string const& name) {
        param = &out[name];
        if (out.size() > parse_settings.object_property_count_limit) {
            throw makeObjectPropertyCountError(
                    parse_settings.object_property_count_limit);
        }
        this->param_key = name;
        this->param_name = name;
        this->depth = 0;
    }

    void indexOp(uint64_t i) {
        if (i >= parse_settings.array_length_limit) {
            throw makeArrayIndexError(this->param_key, parse_settings.array_length_limit);
        }
        switch (param->type()) {
        case Variant::TypeTag::null:
            *param = Variant(VariantVec());
            break;
        case Variant::TypeTag::vec:
            break;
        case Variant::TypeTag::map:
            throw makeMixedTypesError(this->param_key, "vec", "map");
            break;
        default:
            *param = Variant(VariantVec{std::move(*param)});
            break;
        }
        auto& vec = param->modifyVec();
        while (i >= vec.size()) {
            vec.push_back(Variant());
        }
        param = &vec[i];
        this->param_key += "[" + std::to_string(i) + "]";
        incDepth();
    }

    void propertyOp(std::string const& key) {
        switch (param->type()) {
        case Variant::TypeTag::null:
            *param = Variant(VariantMap());
            break;
        case Variant::TypeTag::map:
            break;
        default:
            throw makeMixedTypesError(this->param_key, "map", toString(param->type()));
        }
        auto& map = param->modifyMap();
        param = &map[key];
        if (map.size() > parse_settings.object_property_count_limit) {
            throw makeObjectPropertyCountError(
                    param_key, parse_settings.object_property_count_limit);
        }
        this->param_key += "[" + key + "]";
        incDepth();
    }

    void emptyIndexOp() {
        switch (param->type()) {
        case Variant::TypeTag::null:
            *param = Variant(VariantVec());
            break;
        case Variant::TypeTag::vec:
            break;
        case Variant::TypeTag::map:
            throw makeMixedTypesError(this->param_key, "vec", "map");
        default:
            *param = Variant(VariantVec{*param});
        }
    }

    void val(std::string const& x) {
        valImpl(Variant(x));
    }

    void emptyVal() {
        valImpl(Variant(""));
    }

    void valImpl(Variant x) {
        switch (param->type()) {
        case Variant::TypeTag::null:
            *param = std::move(x);
            break;
        case Variant::TypeTag::vec:
            param->modifyVec().push_back(std::move(x));
            if (param->vec().size() > parse_settings.array_length_limit) {
                throw makeArrayLengthError(this->param_key,
                                           parse_settings.array_length_limit);
            }
            break;
        case Variant::TypeTag::map:
            throw makeMixedTypesError(this->param_key, "map", "string");
        default:
            *param = Variant(VariantVec{std::move(*param), Variant(std::move(x))});
            break;
        }
    }

private:
    qi::rule<Iterator, char()> unreserved;
    qi::rule<Iterator, char()> pct_encoded;
    qi::rule<Iterator, char()> pchar;
    qi::rule<Iterator, char()> sub_delims;
    qi::rule<Iterator, char()> open_bracket;
    qi::rule<Iterator, char()> close_bracket;
    qi::rule<Iterator, char()> plus_as_space;
    qi::rule<Iterator, std::string()> property;
    qi::rule<Iterator, uint16_t()> index;
    qi::rule<Iterator> empty_index;
    qi::rule<Iterator, std::string()> name;
    qi::rule<Iterator> key;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator> empty_value;
    qi::rule<Iterator> parameter;
    qi::rule<Iterator> empty_parameter;
    qi::rule<Iterator> query_string;

    // expectation error
    std::string error_expectation;
    size_t error_expectation_pos;

    // out
    VariantMap out;
    Variant* param{nullptr};
    std::string param_name;
    std::string param_key;
    uint16_t depth;

    // parse settings
    QueryStringParseSettings const parse_settings;
};

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

Variant query_string(std::string const& str,
                     QueryStringParseSettings const& parse_settings) {
    Grammar<std::string::const_iterator> grammar{parse_settings};
    auto const res = qi::parse(str.begin(), str.end(), grammar);
    if (!res) {
        throw QueryStringError("expecting " + grammar.errorExpectation() + " here: \""
                                       + str.substr(grammar.errorExpectationPos()) + "\"",
                               str,
                               grammar.errorExpectation(),
                               grammar.errorExpectationPos());
    }
    return Variant(grammar.result());
}

} // namespace yenxo
