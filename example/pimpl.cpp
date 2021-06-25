#include "pimpl.hpp"

#include <yenxo/pimpl_impl.hpp>

struct X::Impl {};
X::X() = default;
X::~X() = default;
X::X(X const&) = default;
X& X::operator=(X const&) = default;
X::X(X&&) = default;
X& X::operator=(X&&) = default;

int main() {
    X x;
    X y = x;
}
