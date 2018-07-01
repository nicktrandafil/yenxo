#include "variant.hpp"

// local
#include <meta.hpp>
#include <pimpl_impl.hpp>

// std
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


int Variant::integer() const {
    return std::visit(
        rp::Overload{
            [](int x) { return x; },
            [](std::monostate) [[noreturn]]  -> int { throw VariantEmpty(); },
            [](auto&&) [[noreturn]] -> int { throw VariantBadType(); }},
        impl->m);
}


int Variant::integerOr(int x) const {
    return std::visit(
        rp::Overload{
            [](int x) { return x; },
            [&](std::monostate) { return x; },
            [](auto&&) [[noreturn]] -> int { throw VariantBadType(); }},
        impl->m);
}


std::string const& Variant::str() const {
    return std::visit(
        rp::Overload{
            [](std::string const& x) -> std::string const& { return x; },
            [](std::monostate) [[noreturn]] -> std::string const& {
                throw VariantEmpty();
            },
            [](auto&&) [[noreturn]] -> std::string const& {
                throw VariantBadType();
            }},
        impl->m);
}


struct Variant::Vec::Impl {};


Variant::Vec::Vec() = default;


Variant::Vec::~Vec() = default;


Variant::Vec::Vec(Vec const&) = default;


Variant::Vec& Variant::Vec::operator=(Vec const&) = default;


Variant::Vec::Vec(Vec&&) noexcept = default;


Variant::Vec& Variant::Vec::operator=(Vec&&) noexcept = default;


struct Variant::Map::Impl {};


Variant::Map::Map() = default;


Variant::Map::~Map() = default;


Variant::Map::Map(Map const&) = default;


Variant::Map& Variant::Map::operator=(Map const&) = default;


Variant::Map::Map(Map&&) noexcept = default;


Variant::Map& Variant::Map::operator=(Map&&) noexcept = default;
