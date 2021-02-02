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

namespace serialize {

//! @defgroup group-datatypes Data types
//! Data types provided by the library.

//! @defgroup group-exceptions Exceptions
//! Exceptions provided by the library.

//! @defgroup group-traits Type traits
//! The type traits this library is concerned about.

//! @defgroup group-traits-opt-in Opt-in type traits
//! Opt-in traits provided by the library.

//! @defgroup group-function Function objects
//! Function objects provided by the library.

//! @defgroup group-utility Utility
//! Helper utilities provided by the library.

//! @defgroup group-concepts Concepts
//! Concepts provided by the library.

//! @defgroup group-enum Enum
//! @ingroup group-concepts
//! The `Enum` concept represents `enum`-like user-defined types.

//! @defgroup group-struct Struct
//! @ingroup group-concepts
//! The `Struct` concept represents `struct`-like user-defined types.

//! @defgroup group-config Configuration options
//! Configurable options to tweak the global behavior of the library.

//! @defgroup group-meta Meta utility
//! Helper utilities on types

//! @defgroup group-details Details
//! Implementation details. Do not rely on anything here, even if it is
//! documented.

//! @defgroup group-http HTTP utility
//! HTTP utilities.

} // namespace serialize

#include <serialize/comparison_traits.hpp>
#include <serialize/define_enum.hpp>
#include <serialize/exception.hpp>
#include <serialize/string_conversion.hpp>
