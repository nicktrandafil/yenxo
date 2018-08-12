#pragma once


// boost
#include <boost/hana.hpp>


namespace mixin {


///
/// Enables equality comparison for `Derived`
///
template <typename Derived>
struct EqualityComparison {
    friend bool operator==(Derived const& lhs, Derived const& rhs) {
        auto mems = boost::hana::zip(boost::hana::members(lhs),
                                     boost::hana::members(rhs));

        return boost::hana::all_of(
                    mems,
                    boost::hana::fuse([](auto const& lhs, auto const& rhs) {
            return lhs == rhs;
        }));

        return true;
    }

    friend bool operator!=(Derived const& lhs, Derived const& rhs) {
        auto mems = boost::hana::zip(boost::hana::members(lhs),
                                     boost::hana::members(rhs));

        return boost::hana::any_of(
                    mems,
                    boost::hana::fuse([](auto const& lhs, auto const& rhs) {
            return lhs != rhs;
        }));

        return true;
    }

protected:
    ~EqualityComparison() = default;
};


} // namespace mixin
