#include "variant.hpp"

// std
#include <variant>

// local
#include <pimpl.hpp>


using Val = std::variant<
    std::monostate, int, std::string, Variant::Vec, Variant::Map>;


struct Variant::Impl {
    Val m;
};


Variant::Variant() = default;


Variant::Variant(int x)
    : impl(x)
{}


Variant::Variant(std::string const& x)
    : impl(x)
{}


Variant::Variant(std::string&& x)
    : impl(std::move(x))
{}

template <typename T, typename>
Variant::Variant(T&& x) {
}
