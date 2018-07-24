#pragma once


// local
#include <meta.hpp>
#include <type_name.hpp>

// boost
#include <boost/hana.hpp>

// std
#include <ostream>
#include <iomanip>
#include <typeindex>


namespace mixin {


template <typename Derived>
struct OStream {
    friend std::ostream& operator<<(std::ostream& os, Derived const& x) {
        os << typeName<Derived>() << " { ";

        boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
            if constexpr (rp::isOptional(rp::type_c<decltype(value)>)) {
                if (value.has_value()) {
                    os << boost::hana::to<char const*>(name) << ": " << *value << "; ";
                }
            } else {
                os << boost::hana::to<char const*>(name) << ": " << value << "; ";
            }
        }));

        os << "}";

        return os;
    }
};


} // namespace
