#pragma once

#include <serialize/pimpl.hpp>

class X {
public:
    X();
    ~X();

    X(X const&);
    X& operator=(X const&);

    X(X&&);
    X& operator=(X&&);

private:
    struct Impl;
    serialize::Pimpl<Impl> impl;
};
