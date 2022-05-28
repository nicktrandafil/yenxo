/*
  MIT License

  Copyright (c) 2018 Nicolai Trandafil

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <yenxo/meta.hpp>
#include <yenxo/type_name.hpp>
#include <yenxo/variant_conversion.hpp>

#include <boost/hana.hpp>

#include <iomanip>
#include <ostream>

namespace yenxo {
namespace detail {

/// \ingroup group-details
/// Is `std::ostream& operator<<(std::ostream&, T)` defined.
inline constexpr auto const hasOStreamOperator = boost::hana::is_valid(
        [](auto t) -> decltype(operator<<(std::declval<std::ostream>(),
                                          std::declval<typename decltype(t)::type>())) {
        });

} // namespace detail


#ifdef YENXO_DOXYGEN_INVOKED
inline auto oStream = [](std::ostream& os, T const& val) { return return os << val; };
#else
struct OStreamT {
    template <class T>
    void operator()(std::ostream& os, T const&) const;
};

inline constexpr OStreamT oStream;

template <class T, class = void>
struct OStreamImpl : OStreamImpl<T, When<true>> {};

template <class T, bool condition>
struct OStreamImpl<T, When<condition>> {
    void apply(std::ostream&, T const&) {
        static_assert(T::pay_attention_no_ostream_operator_defined_for);
    }
};

template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && isOptional(boost::hana::type_c<T>)>> {
    static void apply(std::ostream& os, T const& val) {
        if (val.has_value()) {
            oStream(os, *val);
        } else {
            os << "None";
        }
    }
};

template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && isContainer(boost::hana::type_c<T>)
                        && isPair(boost::hana::type_c<typename T::value_type>)>> {
    static void apply(std::ostream& os, T const& val) {
        auto const s = size(val);
        std::size_t i = 0;
        os << "{ ";
        for (auto const& x : val) {
            oStream(os, x);
            os << ((i++ == s - 1) ? ";" : "; ");
        }
        os << " }";
    }
};

template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && isContainer(boost::hana::type_c<T>)
                        && !isPair(boost::hana::type_c<typename T::value_type>)>> {
    static void apply(std::ostream& os, T const& val) {
        auto const s = size(val);
        std::size_t i = 0;
        os << "[";
        for (auto const& x : val) {
            oStream(os, x);
            os << ((i++ == s - 1) ? "" : ", ");
        }
        os << "]";
    }
};

template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && isPair(boost::hana::type_c<T>)>> {
    static void apply(std::ostream& os, T const& val) {
        oStream(os, val.first);
        os << ": ";
        oStream(os, val.second);
    }
};

#if YENXO_ENABLE_TYPE_SAFE
template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && strongTypeDef(boost::hana::type_c<T>)>> {
    static void apply(std::ostream& os, T const& val) {
        oStream(os, static_cast<type_safe::underlying_type<decltype(val)>>(val));
    }
};
#endif

template <class T>
struct OStreamImpl<T,
                   When<!detail::hasOStreamOperator(boost::hana::type_c<T>)
                        && isStdVariant(boost::hana::type_c<T>)>> {
    static void apply(std::ostream& os, T const& val) {
        std::visit([&os](auto const& x) { oStream(os, x); }, val);
    }
};

template <class T>
struct OStreamImpl<T, When<detail::hasOStreamOperator(boost::hana::type_c<T>)>> {
    static std::ostream& apply(std::ostream& os, T const& val) {
        return os << val;
    }
};

template <class T>
void OStreamT::operator()(std::ostream& os, T const& val) const {
    OStreamImpl<T>::apply(os, val);
}
#endif

/// \pre `T` should be a Boost.Hana.Struct.
template <class T>
void ostreamImpl(std::ostream& os, T const& x) {
    os << typeName(boost::hana::type_c<T>) << " { ";
    boost::hana::for_each(x, boost::hana::fuse([&](auto name, auto value) {
                              os << boost::hana::to<char const*>(name) << ": ";
                              oStream(os, value);
                              os << "; ";
                          }));
    os << "}";
}

namespace trait {

/// Enables `std::ostream& operator<<(std::ostream&, Derived)` for `Derived`
/// \ingroup group-traits-opt-in
/// \pre `T` should be a Boost.Hana.Struct.
template <typename Derived>
struct OStream {
    friend std::ostream& operator<<(std::ostream& os, Derived const& x) {
        ostreamImpl(os, x);
        return os;
    }
};

} // namespace trait
} // namespace yenxo

/// Enables `std::ostream& operator<<(std::ostream&, T)` for `T`
/// \ingroup group-traits-opt-in
/// \pre `T` should be a Boost.Hana.Struct.
/// \see yenxo::trait::OStream.
#define YENXO_OSTREAM_OPERATOR(T)                                                        \
    friend std::ostream& operator<<(std::ostream& os, T const& x) {                      \
        yenxo::ostreamImpl(os, x);                                                       \
        return os;                                                                       \
    }

/// Enables `std::ostream& operator<<(std::ostream&, T)` for `T`
/// \ingroup group-traits-opt-in
/// \pre `T` should be `toVariantConvertible()`.
///
/// The ostream operator dumps the JSON of the value.
#define YENXO_JSON_OSTREAM_OPERATOR(T)                                                   \
    friend std::ostream& operator<<(std::ostream& os, T const& x) {                      \
        return os << yenxo::toVariant(x).toPrettyJson();                                 \
    }
