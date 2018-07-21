#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL
#include <variant.hpp>
#include <variant_mixins.hpp>
#include <boost/hana.hpp>


namespace hana = boost::hana;


namespace mixin {



} // namespace mixin



struct Person :
        mixin::Var<Person> {
    Person() : age(0) {}

    static auto defaultsMemVals() {
        using namespace hana::literals;

        return hana::make_map(
            hana::make_pair("name"_s, 1)
        );
    }

    BOOST_HANA_DEFINE_STRUCT(
            Person,
            (std::string, name),
            (int, age)
    );
};


int main() {
}
