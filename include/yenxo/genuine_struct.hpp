/*
  MIT License

  Copyright (c) %YEAR% Nicolai Trandafil

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

#include <yenxo/comparison_traits.hpp>
#include <yenxo/ostream_traits.hpp>
#include <yenxo/variant_traits.hpp>

namespace yenxo {

/// Genuine struct
/// \ingroup group-traits-opt-in
///
/// Opts-in `trait::Var`, `trait::EqualityComparison`, `trait::OStream`.
template <class T, class Policy = trait::VarPolicy>
struct GenuineStruct
        : yenxo::trait::Var<T, Policy>
        , yenxo::trait::EqualityComparison<T>
        , yenxo::trait::OStream<T> {};

} // namespace yenxo
