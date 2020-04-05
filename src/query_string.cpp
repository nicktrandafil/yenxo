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

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi    = boost::spirit::qi;
namespace phx   = boost::phoenix;
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

template <class Iterator>
struct Grammar : qi::grammar<Iterator, std::string()> {
    Grammar() : Grammar::base_type(query_string) {
        using qi::alnum;
        using qi::char_;
        using qi::lexeme;
        using qi::lit;
        using qi::xdigit;
        using phx::at_c;

        using namespace qi::labels;

        unreserved %= alnum | char_('-') | char_(".") | char_("_") | char_("~");

        pct_encoded = lexeme[lit('%') >> xdigit >> xdigit]
                            [_val = phx::bind(&byte, at_c<0>(_1), at_c<1>(_1))];

        sub_delims %= char_('!') | char_('$') | char_('&') | char_('\'') | char_('(') |
                      char_(')') | char_('*') | char_('+') | char_(',') | char_(';') |
                      char_('=');

        pchar %= unreserved | pct_encoded | sub_delims | char_(':') | char_('@');

        query_string %= *pchar;
    }

    qi::rule<Iterator, char()       > unreserved;
    qi::rule<Iterator, char()       > pchar;
    qi::rule<Iterator, char()       > pct_encoded;
    qi::rule<Iterator, char()       > sub_delims;
    qi::rule<Iterator, std::string()> query_string;
};

} // namespace

bool query_string(Variant& out, std::string const& str) {
    std::string tmp;
    auto const res = qi::parse(str.begin(), str.end(), Grammar<std::string::const_iterator>(), tmp);
    out = Variant(tmp);
    return res;
}

} // namespace serialize
