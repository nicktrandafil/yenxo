#include <meta.hpp>


namespace {


int foo1();
int foo2(int);


struct Foo {
    void operator()() const;
};


int x = 5;


} // namespace


static_assert(rp::callable(foo1), "");
static_assert(rp::callable(foo2, 5), "");
static_assert(rp::callable(foo2, x), "");
static_assert(!rp::callable(foo2, 5, 6), "");
static_assert(rp::callable(Foo()), "");
static_assert(rp::callable(Foo()), "");



