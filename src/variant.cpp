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


#include <variant.hpp>

// local
#include <meta.hpp>
#include <pimpl_impl.hpp>

// std
#include <unordered_map>
#include <vector>
#include <variant>


using Val = std::variant<
    std::monostate,
    short int,
    unsigned short int,
    int,
    unsigned int,
    double,
    std::string,
    Variant::Vec,
    Variant::Map>;


struct Variant::Impl {
    Val m;
};


Variant::Variant() = default;


Variant::~Variant() = default;


Variant::Variant(short int x) : impl(x) {}
Variant::Variant(unsigned short int x) : impl(x) {}
Variant::Variant(int x) : impl(x) {}
Variant::Variant(unsigned int x) : impl(x) {}
Variant::Variant(double x) : impl(x) {}


Variant::Variant(std::string const& x) : impl(x) {}
Variant::Variant(std::string&& x) : impl(std::move(x)) {}


Variant::Variant(Vec const& x) : impl(x) {}
Variant::Variant(Vec&& x) : impl(std::move(x)) {}


Variant::Variant(Map const& x) : impl(x) {}
Variant::Variant(Map&& x) : impl(std::move(x)) {}


Variant::Variant(Variant const&) = default;


Variant& Variant::operator=(Variant const&) = default;


Variant::Variant(Variant&&) noexcept = default;


Variant& Variant::operator=(Variant&&) noexcept = default;


namespace {


template <typename T, typename = void>
struct GetHelper : GetHelper<T, When<true>> {};


template <typename T, bool condition>
struct GetHelper<T, When<condition>> {
    T const& operator()(T const& x) const noexcept { return x; }
    [[noreturn]] T const& operator()(std::monostate) const {
        throw VariantEmpty();
    }
    template <typename U>
    [[noreturn]] T const& operator()(U const&) const { throw VariantBadType(); }
};


template <typename T>
struct GetHelper<T, When<std::is_integral_v<T>>> {
    T operator()(T x) const noexcept { return x; }
    [[noreturn]]  T operator()(std::monostate) const { throw VariantEmpty(); }
    template <typename U>
    [[noreturn]] T operator()(U) const { throw VariantBadType(); }
};


template <typename T>
struct GetOrHelper {
    T operator()(T x) const { return x; }
    T operator()(std::monostate) const { return t; }
    template <typename U>
    [[noreturn]] T operator()(U const&) const { throw VariantBadType(); }
    T const& t;
};


} // namespace


short int Variant::shortInt() const {
    return std::visit(GetHelper<short int>(), impl->m);
}


short int Variant::shortIntOr(short int x) const {
    return std::visit(GetOrHelper<short int>{x}, impl->m);
}


unsigned short int Variant::ushortInt() const {
    return std::visit(GetHelper<unsigned short int>(), impl->m);
}


unsigned short int Variant::ushortIntOr(unsigned short int x) const {
    return std::visit(GetOrHelper<unsigned short int>{x}, impl->m);
}


int Variant::integer() const {
    return std::visit(GetHelper<int>(), impl->m);
}


int Variant::integerOr(int x) const {
    return std::visit(GetOrHelper<int>{x}, impl->m);
}


unsigned int Variant::uint() const {
    return std::visit(GetHelper<unsigned int>(), impl->m);
}


unsigned int Variant::uintOr(unsigned int x) const {
    return std::visit(GetOrHelper<unsigned int>{x}, impl->m);
}


double Variant::floating() const {
    return std::visit(GetHelper<double>(), impl->m);
}


double Variant::floatingOr(double x) const {
    return std::visit(GetOrHelper<double>{x}, impl->m);
}


std::string const& Variant::str() const {
    return std::visit(GetHelper<std::string>(), impl->m);
}


std::string Variant::strOr(std::string const& x) const {
    return std::visit(GetOrHelper<std::string>{x}, impl->m);
}


Variant::Vec const& Variant::vec() const {
    return std::visit(GetHelper<Vec>(), impl->m);
}


Variant::Vec Variant::vecOr(Vec const& x) const {
    return std::visit(GetOrHelper<Vec>{x}, impl->m);
}


Variant::Map const& Variant::map() const {
    return std::visit(GetHelper<Map>(), impl->m);
}


Variant::Map Variant::mapOr(Map const& x) const {
    return std::visit(GetOrHelper<Map>{x}, impl->m);
}


bool Variant::operator==(Variant const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


bool Variant::operator!=(Variant const& rhs) const noexcept {
    return impl->m != rhs.impl->m;
}
