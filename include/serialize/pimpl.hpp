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


#pragma once


// std
#include <memory>
#include <type_traits>


namespace serialize {


///
/// Generic 'Pimpl' idiom implementation
///
/// Throwing member functions do throw if and only if `T` or allocation throws
///
/// Example
/// -------
///
/// x.hpp
/// -----
///
/// #include <pimpl.hpp>
///
/// class X {
/// 	X();
/// 	~X();
///
/// 	X(X const&);
/// 	X& operator=(X const&);
///
/// 	X(X&&);
/// 	X& operator=(X&&);
/// private:
/// 	struct Impl;
/// 	Pimpl<Impl> impl;
/// };
///
/// x.cpp
/// -----
///
/// #include "x.hpp"
/// #include <pimpl_impl.hpp>
///
/// struct X::Impl {};
/// X::X() = default;
/// X::~X() = default;
/// X::X(X const&) = default;
/// X& X::operator=(X const&) = default;
/// X::X(X&&) = default;
/// X& X::operator=(X&&) = default;
///
template <typename T>
class Pimpl {
public:
    Pimpl();
    ~Pimpl();

    // copy

    Pimpl(Pimpl const&);
    // \post Strong exception guarantee
    Pimpl& operator=(Pimpl const& rhs);

    // move
    Pimpl(Pimpl&&) noexcept;
    Pimpl& operator=(Pimpl&&) noexcept;

    //
    // Agregation initialization
    // SFINAE trick to not confuse with copy constructor
    //

    template <typename U,
              typename =
                  typename std::enable_if<
                      !std::is_same<Pimpl, U>::value, void>::type>
    explicit Pimpl(U&& u);

    template <typename T1, typename T2, typename ...Args>
    Pimpl(T1&& a1, T2&& a2, Args&&... args);

    //
    // Dereferencing
    //

    T* operator->() noexcept;
    T const* operator->() const noexcept;

    T& operator*() noexcept;
    T const& operator*() const noexcept;

private:
    std::unique_ptr<T> m;
};


}
