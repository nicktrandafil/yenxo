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
#include <type_name.hpp>

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


template <typename A, typename B>
constexpr auto same_sign_v = !(std::is_signed_v<A> ^ std::is_signed_v<B>);


///
/// Check if a specific value of type `U` is representable in type `T`
///
template <typename T, typename U, typename = void>
struct IntegralCheckedCast : IntegralCheckedCast<T, U, When<true>> {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>);
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<T, U, When<same_sign_v<U, T> &&
                                      (sizeof(T) >= sizeof(U))>> {
    T operator()(U x) const noexcept { return x; }
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<T, U, When<std::is_signed_v<T> &&
                                      std::is_unsigned_v<U> &&
                                      (sizeof(T) > sizeof(U))>> {
    T operator()(U x) const noexcept { return x; }
};

template <typename T, typename U>
struct IntegralCheckedCast<T, U, When<std::is_signed_v<T> &&
                                      std::is_unsigned_v<U> &&
                                      (sizeof(T) <= sizeof(U))>> {
    T operator()(U x) const {

#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

        if (x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                        std::string(unqualifiedTypeName<T>()),
                        std::to_string(x));
        }

        return x;
    }
};

template <typename T, typename U>
struct IntegralCheckedCast<T, U, When<std::is_unsigned_v<T> &&
                                      std::is_signed_v<U>>> {
    T operator()(U x) const {

#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

        if (x < 0 || x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                        std::string(unqualifiedTypeName<T>()),
                        std::to_string(x));
        }

        return x;
    }
};


template <typename T, typename U, bool condition>
struct IntegralCheckedCast<T, U, When<condition>> {
    T operator()(U x) const {

#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

       if (x < std::numeric_limits<T>::min() ||
           x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                        std::string(unqualifiedTypeName<T>()),
                        std::to_string(x));
        }
        return x;
    }
};


template <typename T, typename U>
constexpr IntegralCheckedCast<T, U> integralCheckedCast;


template <typename T>
struct GetHelper<T, When<std::is_integral_v<T>>> {
    [[noreturn]] T operator()(std::monostate) const { throw VariantEmpty(); }

    T operator()(short int x)          const {
        return integralCheckedCast<T, decltype(x)>(x);
    }

    T operator()(unsigned short int x) const {
        return integralCheckedCast<T, decltype(x)>(x);
    }

    T operator()(int x)                const {
        return integralCheckedCast<T, decltype(x)>(x);
    }

    T operator()(unsigned int x)       const {
        return integralCheckedCast<T, decltype(x)>(x);
    }

    T operator()(double)       const { throw VariantBadType(); }
    T operator()(std::string)  const { throw VariantBadType(); }
    T operator()(Variant::Vec) const { throw VariantBadType(); }
    T operator()(Variant::Map) const { throw VariantBadType(); }
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
