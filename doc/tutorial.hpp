/*
  MIT License

  Copyright (c) 2021 Nicolai Trandafil

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

namespace yenxo {
/*!

@mainpage User Manual

@tableofcontents

@section tutorial-description Description

Yenxo tries to reduce your application's boilerplate code when it comes to serializing
and deserializing C++ values into text-based data interchange formats, like JSON.

The central type of Yenxo is Variant. A value first gets converted into Variant, then
the Variant value can be converted to the desired text-based data interchange format.
Variant natively supports JSON format, but one can traverse Variant value and serialize it
to any desired text-based data interchange format.

@warning The fact that serialization process has an intermediate value (Variant) implies
some overhead.

Having an intermediate value in serialization process has also a benefit - code reusing.
One can use the same code to convert a value into YAML and JSON.

But the main reason for having an intermediate value is to decouple the library from a
specific data interchange format and the format implementation library and to build
Yenxo around its own type for internal representation of data interchange format.

<div style="text-align: center; margin: 50px auto;">
    <img src="serialize_stages.png">
    <div style="font-weight: bold;">Figure 1. Serialization stages.</div>
</div>

This library and documentation are more concerned about the stage 1 from Figure 1,
therefore stage 2 is going to get less attention.

@section tutorial-main-api Main API

The main API of this library consists of just two function objects:
1. toVariant();
2. fromVariant().

These two function objects are enabled for any C++ type that supports
toVariantConvertible() and fromVariantConvertible() type traits. In the related
documentation you'll find what types support the traits and how to enable the traits
for user defined types.

@section tutorial-reducing-boilerplate-code Reducing boilerplate code

@subsection tutorial-opt-in-var Opt-in trait::Var

You can automatically enable `toVariantConvertible()` and `fromVariantConvertible()`
for your struct using opt-in `trait::Var`. The opt-in trait implements static methods
`Variant toVariant(T)` and `T fromVariant(Variant)` inside your type if:
1. every member of the struct already supports `toVariantConvertible()` and
`fromVariantConvertible()`;
2. the struct is
[Boost.Hana.Struct](https://www.boost.org/doc/libs/1_71_0/libs/hana/doc/html/group__group-Struct.html).

Define a struct,

@code{cpp}
struct Person : trait::Var<Person> {
    BOOST_HANA_DEFINE_STRUCT(Person
        , (std::string, first_name)
        , (std::string, last_name )
        , (uint8_t    , age       )
    )
};
@endcode

not having any boilerplate code, enjoy serializing

@code{cpp}
Person p;
p.first_name = "First name";
p.last_name = "Last name";
p.age = 0;
Variant const var = toVariant(p);
@endcode

@note Aggregate initialization is unavailable because Person inherits something.

and deserializing
@code{cpp}
VariantMap const var{
    {"first_name", "First name"},
    {"last_name" , "Last name" },
    {"age"       , 0           },
};
auto const p = fromVariant<Person>(var);
@endcode

You will save aggregate initialization if you prefer macro opt-ins `YENXO_TO_VARIANT()`
and `YENXO_FROM_VARIANT()` to `trait::Var`:
@code{cpp}
struct Person {
    YENXO_TO_VARIANT(Person)
    YENXO_FROM_VARIANT(Person)

    BOOST_HANA_DEFINE_STRUCT(Person
        , (std::string, first_name)
        , (std::string, last_name )
        , (uint8_t    , age       )
    )
};
@endcode

[Boost.Hana.Struct](https://www.boost.org/doc/libs/1_71_0/libs/hana/doc/html/group__group-Struct.html)
can be defined in a non-intrusive way:
@code{cpp}
struct Person {
    YENXO_TO_VARIANT(Person)
    YENXO_FROM_VARIANT(Person)

    std::string first_name;
    std::string last_name;
    uint8_t age;
};

BOOST_HANA_ADAPT_STRUCT(Person, first_name, last_name, age);
@endcode

*/
}
