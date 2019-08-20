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


#include <serialize/meta.hpp>


using namespace boost::hana;


using namespace serialize;


#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wundefined-internal"
#pragma GCC diagnostic ignored "-Wunneeded-member-function"


namespace {


int foo1();
int foo2(int);


struct Foo {
    void operator()() const;
};


} // namespace


#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif


static_assert(callable(foo1));
static_assert(callable(foo2, type_c<int>));
static_assert(!callable(foo2, type_c<int>, type_c<int>));
static_assert(callable(Foo()));
static_assert(callable(Foo()));
static_assert(isIterable(type_c<std::vector<int>>));
static_assert(isString(type_c<std::string>));
static_assert(isPair(type_c<std::pair<int, std::string>>));
static_assert(!isKeyValue(type_c<std::pair<int, std::string>>));
static_assert(isKeyValue(type_c<std::pair<std::string, std::string>>));
static_assert(S<int, char, bool>::convertible<int>());
