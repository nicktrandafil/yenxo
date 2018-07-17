#include <meta.hpp>


namespace {


int foo1();
int foo2(int);


struct Foo {
    void operator()() const;
};


} // namespace


static_assert(rp::callable(foo1), "");
static_assert(rp::callable(foo2, rp::Type<int>()), "");
static_assert(!rp::callable(foo2, rp::Type<int>(), rp::Type<int>()), "");
static_assert(rp::callable(Foo()), "");
static_assert(rp::callable(Foo()), "");



