#include "variant.hpp"

// local
#include <meta.hpp>
#include <pimpl_impl.hpp>
#include <vector>

// std
#include <unordered_map>
#include <variant>


using Val = std::variant<
    std::monostate,
    int,
    std::string,
    Variant::Vec,
    Variant::Map>;


struct Variant::Impl {
    Val m;
};


Variant::Variant() = default;


Variant::~Variant() = default;


Variant::Variant(int x)
    : impl(x)
{}


Variant::Variant(std::string const& x)
    : impl(x)
{}


Variant::Variant(std::string&& x)
    : impl(std::move(x))
{}


Variant::Variant(Vec const& x)
    : impl(x)
{}


Variant::Variant(Vec&& x)
    : impl(std::move(x))
{}


Variant::Variant(Map const& x)
    : impl(x)
{}


Variant::Variant(Map&& x)
    : impl(std::move(x))
{}


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


int Variant::integer() const {
    return std::visit(GetHelper<int>(), impl->m);
}


int Variant::integerOr(int x) const {
    return std::visit(GetOrHelper<int>{x}, impl->m);
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


Variant::TypeId Variant::type() const noexcept {
    return std::visit(rp::Overload{
                          [](std::monostate) noexcept { return TypeId::No; },
                          [](int) noexcept { return TypeId::Int; },
                          [](std::string const&) noexcept { return TypeId::Str; },
                          [](Vec const&) noexcept { return TypeId::Vec; },
                          [](Map const&) noexcept { return TypeId::Map; }
                      },
                      impl->m);
}
