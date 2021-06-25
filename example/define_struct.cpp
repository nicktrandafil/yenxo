#include <yenxo/define_struct.hpp>
#include <yenxo/variant_traits.hpp>

#include <boost/hana/assert.hpp>

using namespace std;
using namespace yenxo;

struct A {
    DEFINE_STRUCT(A,
                  (int, a),
                  (int, b, Default(10)),
                  (double, c, Name("cc")),
                  (string, d, Name("dd"), Default("ab")),
                  (bool, e, Default(false), Name("flag")));
};

int main() {
    A s;
    s.a = 10;

    BOOST_HANA_RUNTIME_CHECK(boost::hana::find(s, BOOST_HANA_STRING("a"))
                             == boost::hana::just(10));

    BOOST_HANA_RUNTIME_CHECK(A::defaults()[BOOST_HANA_STRING("b")] == 10);
    BOOST_HANA_RUNTIME_CHECK(A::names()[BOOST_HANA_STRING("d")] == string("dd"));
}
