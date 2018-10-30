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


namespace rp {


template <typename T>
Pimpl<T>::Pimpl() : m(new T) {}


template <typename T>
Pimpl<T>::~Pimpl() = default;


template <typename T>
Pimpl<T>::Pimpl(Pimpl const& rhs) : m(new T(*rhs.m)) {}


template <typename T>
Pimpl<T>::Pimpl(Pimpl&&) noexcept = default;


template <typename T>
Pimpl<T>& Pimpl<T>::operator=(Pimpl const& rhs) {
    Pimpl<T> tmp(rhs);
    *this = std::move(tmp);
    return *this;
}


template <typename T>
Pimpl<T>& Pimpl<T>::operator=(Pimpl<T>&&) noexcept = default;


template <typename T>
template <typename U, typename>
Pimpl<T>::Pimpl(U&& u) : m(new T{std::forward<U>(u)}) {}


template <typename T>
template <typename U1, typename U2, typename ...Args>
Pimpl<T>::Pimpl(U1&& u1, U2&& u2, Args&&... args)
    : m(new T{std::forward<U1>(u1),
              std::forward<U2>(u2),
              std::forward<Args>(args)...
            })
{}


template <typename T>
T* Pimpl<T>::operator->() noexcept { return m.get(); }


template <typename T>
T const* Pimpl<T>::operator->() const noexcept { return m.get(); }


template <typename T>
T& Pimpl<T>::operator*() noexcept { return *m; }


template <typename T>
T const& Pimpl<T>::operator*() const noexcept { return *m; }


}
