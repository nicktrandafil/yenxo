/*
  MIT License

  Copyright (c) 2019 Nicolai Trandafil

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


#pragma once


/// Is macros argument empty
/// Examples:
/// 	ISEMPTY() -> 1
/// 	ISEMPTY(1) -> 0
/// 	ISEMPTY(,) -> 0
#define ISEMPTY(...) \
ISEMPTY_( \
          /* test if there is just one argument, eventually an empty
             one */ \
          HAS_COMMA(__VA_ARGS__), \
          /* test if TRIGGER_PARENTHESIS_ together with the argument
             adds a comma */ \
          HAS_COMMA(TRIGGER_PARENTHESIS_ __VA_ARGS__), \
          /* test if the argument together with a parenthesis
             adds a comma */ \
          HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
          /* test if placing it between TRIGGER_PARENTHESIS_ and the
             parenthesis adds a comma */ \
          HAS_COMMA(TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
          )

/// Determines if macros argument is a tuple
/// Examples:
/// 	IS_TUPLE() -> 0
/// 	IS_TUPLE(1) -> 0
/// 	IS_TUPLE((1)) -> 1
#define IS_TUPLE(x) \
    HAS_COMMA(TRIGGER_PARENTHESIS_ x)

#define ARG16_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) ARG16_(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0)
#define TRIGGER_PARENTHESIS_(...) ,

#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define ISEMPTY_(_0, _1, _2, _3) HAS_COMMA(PASTE5(IS_EMPTY_CASE_, _0, _1, _2, _3))
#define IS_EMPTY_CASE_0001 ,
