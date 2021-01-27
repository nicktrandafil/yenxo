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

namespace serialize {
/*!

@mainpage User Manual

@tableofcontents

@section tutorial-description Description

Serialize tries to reduce your application's boilerplate code when it comes to serializing
and deserializing C++ values into text-based data interchange formats, like JSON.

The central type of Serialize is Variant. A value first gets converted into Variant, then
the Variant value can be converted to the desired text-based data interchange format.
Variant natively supports JSON format, but one can traverse Variant value and serialize it
to any desired text-based data interchange format.

\warning The fact that serialization process has an intermediate value (Variant) implies
some overhead.

Having an intermediate value in serialization process has also a benefit - code reusing.
One can use the same code to convert a value into YAML and JSON.

But the main reason for having an intermediate value is to decouple the library from a
specific data interchange format and the format implementation library and to build
Serialize around its own type for internal representation of data interchange format.

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

*/
}
