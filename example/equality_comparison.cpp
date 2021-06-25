#include <yenxo/comparison_traits.hpp>

#include <boost/hana/assert.hpp>

#include <string>

using namespace std;
using namespace yenxo;

struct Foo : trait::EqualityComparison<Foo> {
    Foo(int a, string b)
            : a(a)
            , b(b) {
    }
    BOOST_HANA_DEFINE_STRUCT(Foo, (int, a), (string, b));
};

int main() {
    Foo x1{0, "abc"};
    Foo x2{0, "abc"};
    BOOST_HANA_RUNTIME_CHECK(x1 == x2);
    x2.b = "d";
    BOOST_HANA_RUNTIME_CHECK(x1 != x2);
}
