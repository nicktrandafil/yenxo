/*
  MIT License

  Copyright (c) 2019 Nicolai Trandafil

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

#include <yenxo/preprocessor.hpp>
#include <yenxo/value_tag.hpp>

#include <boost/hana/detail/struct_macros.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/map.hpp>

#include <type_traits>

namespace yenxo::detail {

template <class T, size_t i>
constexpr auto memberName() {
    return boost::hana::at_c<i>(
            boost::hana::transform(T::hana_accessors_impl::apply(), boost::hana::first));
}

struct Ignore {};

} // namespace yenxo::detail

// Nth element of 2-tuple
#define NTH1OF2s(a, b) a
#define NTH2OF2s(a, b) b

#define EVALs(...) __VA_ARGS__

// Given a tuple (a, b, c, d)      || (a, b, c)             || (a, b)
// produces      ((a, b), (c, d))  || ((a, b), (c, Ignore)) || ((a, b), (Ignore, Ignore))
// \note requires at least 2 arguments and at most 4
#define RESOLVE_VALUEs(...)                                                              \
    RESOLVE_VALUEs_IMPL(BOOST_HANA_PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define RESOLVE_VALUEs_IMPL(N, ...)                                                      \
    BOOST_HANA_PP_CONCAT(RESOLVE_VALUEs_IMPL_, N)                                        \
    (__VA_ARGS__)
#define RESOLVE_VALUEs_IMPL_2(type, name)                                                \
    ((type, name), (yenxo::detail::Ignore(), yenxo::detail::Ignore()))
#define RESOLVE_VALUEs_IMPL_3(type, name, m1)                                            \
    ((type, name), (m1, yenxo::detail::Ignore()))
#define RESOLVE_VALUEs_IMPL_4(type, name, m1, m2) ((type, name), (m1, m2))

#define EXTRs(t) EXTRs_EXTR t
#define EXTRs_EXTR(t, ...) (EXTRs_IMPL(IS_TUPLE(t), t), __VA_ARGS__)
#define EXTRs_IMPL(_01, t)                                                               \
    BOOST_HANA_PP_CONCAT(EXTRs_IMPL_, _01)                                               \
    (t)
#define EXTRs_IMPL_0(t) t // not tuple
#define EXTRs_IMPL_1(t) EVALs t

/// Generates a Boost.Hana.Struct adding some meta-information as static members
/// \ingroup group-struct
///
/// Effectively forwards first two arguments of tuple arguments
/// to `BOOST_HANA_DEFINE_STRUCT`. Extra arguments of the tuple
/// arguments (`Name` and `Default`) compose two maps returned by
/// generated static methods `defaults()` and `names()`. Keys in the maps are
/// member names (as hana strings). Values are the arguments passed to
/// `Default` and `Name`. Types `Default` and `Name` are just tags and do not
/// appear in the generated code.
///
/// Example
/// -------
/// @include example/define_struct.cpp
#ifdef YENXO_DOXYGEN_INVOKED
auto DEFINE_STRUCT(...) = ;
#define DEFINE_STRUCT(Type, ...) see documentation
#else
#define DEFINE_STRUCT(...)                                                               \
    DEFINE_STRUCT_IMPL(BOOST_HANA_PP_NARG(__VA_ARGS__), __VA_ARGS__)
#endif

#define DEFINE_STRUCT_IMPL(N, ...)                                                       \
    BOOST_HANA_PP_CONCAT(DEFINE_STRUCT_IMPL_, N)                                         \
    (__VA_ARGS__)

#define DEFINE_STRUCT_IMPL_1(Type) BOOST_HANA_DEFINE_STRUCT_IMPL_1(Type)

#define DEFINE_STRUCT_IMPL_2(Type, m1) DEFINE_STRUCT_IMPL_2_(Type, RESOLVE_VALUEs m1)
#define DEFINE_STRUCT_IMPL_2_(Type, m1)                                                  \
    BOOST_HANA_DEFINE_STRUCT_IMPL_2(Type, EXTRs(NTH1OF2s m1));                           \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_3(Type, m1, m2)                                               \
    DEFINE_STRUCT_IMPL_3_(Type, RESOLVE_VALUEs m1, RESOLVE_VALUEs m2)
#define DEFINE_STRUCT_IMPL_3_(Type, m1, m2)                                              \
    BOOST_HANA_DEFINE_STRUCT_IMPL_3(Type, EXTRs(NTH1OF2s m1), EXTRs(NTH1OF2s m2));       \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_4(Type, m1, m2, m3)                                           \
    DEFINE_STRUCT_IMPL_4_(Type, RESOLVE_VALUEs m1, RESOLVE_VALUEs m2, RESOLVE_VALUEs m3)
#define DEFINE_STRUCT_IMPL_4_(Type, m1, m2, m3)                                          \
    BOOST_HANA_DEFINE_STRUCT_IMPL_4(                                                     \
            Type, EXTRs(NTH1OF2s m1), EXTRs(NTH1OF2s m2), EXTRs(NTH1OF2s m3));           \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_5(Type, m1, m2, m3, m4)                                       \
    DEFINE_STRUCT_IMPL_5_(Type,                                                          \
                          RESOLVE_VALUEs m1,                                             \
                          RESOLVE_VALUEs m2,                                             \
                          RESOLVE_VALUEs m3,                                             \
                          RESOLVE_VALUEs m4)
#define DEFINE_STRUCT_IMPL_5_(Type, m1, m2, m3, m4)                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_5(Type,                                                \
                                    EXTRs(NTH1OF2s m1),                                  \
                                    EXTRs(NTH1OF2s m2),                                  \
                                    EXTRs(NTH1OF2s m3),                                  \
                                    EXTRs(NTH1OF2s m4));                                 \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_6(Type, m1, m2, m3, m4, m5)                                   \
    DEFINE_STRUCT_IMPL_6_(Type,                                                          \
                          RESOLVE_VALUEs m1,                                             \
                          RESOLVE_VALUEs m2,                                             \
                          RESOLVE_VALUEs m3,                                             \
                          RESOLVE_VALUEs m4,                                             \
                          RESOLVE_VALUEs m5)
#define DEFINE_STRUCT_IMPL_6_(Type, m1, m2, m3, m4, m5)                                  \
    BOOST_HANA_DEFINE_STRUCT_IMPL_6(Type,                                                \
                                    EXTRs(NTH1OF2s m1),                                  \
                                    EXTRs(NTH1OF2s m2),                                  \
                                    EXTRs(NTH1OF2s m3),                                  \
                                    EXTRs(NTH1OF2s m4),                                  \
                                    EXTRs(NTH1OF2s m5));                                 \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_7(Type, m1, m2, m3, m4, m5, m6)                               \
    DEFINE_STRUCT_IMPL_7_(Type,                                                          \
                          RESOLVE_VALUEs m1,                                             \
                          RESOLVE_VALUEs m2,                                             \
                          RESOLVE_VALUEs m3,                                             \
                          RESOLVE_VALUEs m4,                                             \
                          RESOLVE_VALUEs m5,                                             \
                          RESOLVE_VALUEs m6)
#define DEFINE_STRUCT_IMPL_7_(Type, m1, m2, m3, m4, m5, m6)                              \
    BOOST_HANA_DEFINE_STRUCT_IMPL_7(Type,                                                \
                                    EXTRs(NTH1OF2s m1),                                  \
                                    EXTRs(NTH1OF2s m2),                                  \
                                    EXTRs(NTH1OF2s m3),                                  \
                                    EXTRs(NTH1OF2s m4),                                  \
                                    EXTRs(NTH1OF2s m5),                                  \
                                    EXTRs(NTH1OF2s m6));                                 \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_8(Type, m1, m2, m3, m4, m5, m6, m7)                           \
    DEFINE_STRUCT_IMPL_8_(Type,                                                          \
                          RESOLVE_VALUEs m1,                                             \
                          RESOLVE_VALUEs m2,                                             \
                          RESOLVE_VALUEs m3,                                             \
                          RESOLVE_VALUEs m4,                                             \
                          RESOLVE_VALUEs m5,                                             \
                          RESOLVE_VALUEs m6,                                             \
                          RESOLVE_VALUEs m7)
#define DEFINE_STRUCT_IMPL_8_(Type, m1, m2, m3, m4, m5, m6, m7)                          \
    BOOST_HANA_DEFINE_STRUCT_IMPL_8(Type,                                                \
                                    EXTRs(NTH1OF2s m1),                                  \
                                    EXTRs(NTH1OF2s m2),                                  \
                                    EXTRs(NTH1OF2s m3),                                  \
                                    EXTRs(NTH1OF2s m4),                                  \
                                    EXTRs(NTH1OF2s m5),                                  \
                                    EXTRs(NTH1OF2s m6),                                  \
                                    EXTRs(NTH1OF2s m7));                                 \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_9(Type, m1, m2, m3, m4, m5, m6, m7, m8)                       \
    DEFINE_STRUCT_IMPL_9_(Type,                                                          \
                          RESOLVE_VALUEs m1,                                             \
                          RESOLVE_VALUEs m2,                                             \
                          RESOLVE_VALUEs m3,                                             \
                          RESOLVE_VALUEs m4,                                             \
                          RESOLVE_VALUEs m5,                                             \
                          RESOLVE_VALUEs m6,                                             \
                          RESOLVE_VALUEs m7,                                             \
                          RESOLVE_VALUEs m8)
#define DEFINE_STRUCT_IMPL_9_(Type, m1, m2, m3, m4, m5, m6, m7, m8)                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_9(Type,                                                \
                                    EXTRs(NTH1OF2s m1),                                  \
                                    EXTRs(NTH1OF2s m2),                                  \
                                    EXTRs(NTH1OF2s m3),                                  \
                                    EXTRs(NTH1OF2s m4),                                  \
                                    EXTRs(NTH1OF2s m5),                                  \
                                    EXTRs(NTH1OF2s m6),                                  \
                                    EXTRs(NTH1OF2s m7),                                  \
                                    EXTRs(NTH1OF2s m8));                                 \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_10(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9)                  \
    DEFINE_STRUCT_IMPL_10_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9)
#define DEFINE_STRUCT_IMPL_10_(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9)                 \
    BOOST_HANA_DEFINE_STRUCT_IMPL_10(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9));                                \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)));                   \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_11(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10)             \
    DEFINE_STRUCT_IMPL_11_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10)
#define DEFINE_STRUCT_IMPL_11_(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10)            \
    BOOST_HANA_DEFINE_STRUCT_IMPL_11(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_12(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11)        \
    DEFINE_STRUCT_IMPL_12_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11)
#define DEFINE_STRUCT_IMPL_12_(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11)       \
    BOOST_HANA_DEFINE_STRUCT_IMPL_12(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_13(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12)   \
    DEFINE_STRUCT_IMPL_13_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12)
#define DEFINE_STRUCT_IMPL_13_(Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12)  \
    BOOST_HANA_DEFINE_STRUCT_IMPL_13(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_14(                                                           \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13)                    \
    DEFINE_STRUCT_IMPL_14_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13)
#define DEFINE_STRUCT_IMPL_14_(                                                          \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13)                    \
    BOOST_HANA_DEFINE_STRUCT_IMPL_14(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_15(                                                           \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14)               \
    DEFINE_STRUCT_IMPL_15_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14)
#define DEFINE_STRUCT_IMPL_15_(                                                          \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14)               \
    BOOST_HANA_DEFINE_STRUCT_IMPL_15(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_16(                                                           \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15)          \
    DEFINE_STRUCT_IMPL_16_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15)
#define DEFINE_STRUCT_IMPL_16_(                                                          \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15)          \
    BOOST_HANA_DEFINE_STRUCT_IMPL_16(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_17(                                                           \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16)     \
    DEFINE_STRUCT_IMPL_17_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16)
#define DEFINE_STRUCT_IMPL_17_(                                                          \
        Type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16)     \
    BOOST_HANA_DEFINE_STRUCT_IMPL_17(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_18(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17)                                                       \
    DEFINE_STRUCT_IMPL_18_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17)
#define DEFINE_STRUCT_IMPL_18_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_18(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_19(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18)                                                       \
    DEFINE_STRUCT_IMPL_19_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18)
#define DEFINE_STRUCT_IMPL_19_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_19(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_20(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19)                                                       \
    DEFINE_STRUCT_IMPL_20_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19)
#define DEFINE_STRUCT_IMPL_20_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_20(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_21(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20)                                                       \
    DEFINE_STRUCT_IMPL_21_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20)
#define DEFINE_STRUCT_IMPL_21_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_21(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_22(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21)                                                       \
    DEFINE_STRUCT_IMPL_22_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21)
#define DEFINE_STRUCT_IMPL_22_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_22(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_23(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22)                                                       \
    DEFINE_STRUCT_IMPL_23_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22)
#define DEFINE_STRUCT_IMPL_23_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_23(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_24(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23)                                                       \
    DEFINE_STRUCT_IMPL_24_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23)
#define DEFINE_STRUCT_IMPL_24_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_24(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_25(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24)                                                       \
    DEFINE_STRUCT_IMPL_25_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24)
#define DEFINE_STRUCT_IMPL_25_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_25(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_26(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25)                                                       \
    DEFINE_STRUCT_IMPL_26_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25)
#define DEFINE_STRUCT_IMPL_26_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_26(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_27(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26)                                                       \
    DEFINE_STRUCT_IMPL_27_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26)
#define DEFINE_STRUCT_IMPL_27_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_27(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_28(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27)                                                       \
    DEFINE_STRUCT_IMPL_28_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27)
#define DEFINE_STRUCT_IMPL_28_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_28(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_29(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28)                                                       \
    DEFINE_STRUCT_IMPL_29_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28)
#define DEFINE_STRUCT_IMPL_29_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_29(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_30(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29)                                                       \
    DEFINE_STRUCT_IMPL_30_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29)
#define DEFINE_STRUCT_IMPL_30_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_30(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_31(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30)                                                       \
    DEFINE_STRUCT_IMPL_31_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30)
#define DEFINE_STRUCT_IMPL_31_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_31(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_32(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31)                                                       \
    DEFINE_STRUCT_IMPL_32_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31)
#define DEFINE_STRUCT_IMPL_32_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_32(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_33(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32)                                                       \
    DEFINE_STRUCT_IMPL_33_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32)
#define DEFINE_STRUCT_IMPL_33_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_33(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_34(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32,                                                       \
                              m33)                                                       \
    DEFINE_STRUCT_IMPL_34_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32,                                           \
                           RESOLVE_VALUEs m33)
#define DEFINE_STRUCT_IMPL_34_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32,                                                      \
                               m33)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_34(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32),                                \
                                     EXTRs(NTH1OF2s m33));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 32>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m33),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m33)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_35(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32,                                                       \
                              m33,                                                       \
                              m34)                                                       \
    DEFINE_STRUCT_IMPL_35_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32,                                           \
                           RESOLVE_VALUEs m33,                                           \
                           RESOLVE_VALUEs m34)
#define DEFINE_STRUCT_IMPL_35_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32,                                                      \
                               m33,                                                      \
                               m34)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_35(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32),                                \
                                     EXTRs(NTH1OF2s m33),                                \
                                     EXTRs(NTH1OF2s m34));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 32>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m33),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m33)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 33>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m34),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m34)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_36(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32,                                                       \
                              m33,                                                       \
                              m34,                                                       \
                              m35)                                                       \
    DEFINE_STRUCT_IMPL_36_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32,                                           \
                           RESOLVE_VALUEs m33,                                           \
                           RESOLVE_VALUEs m34,                                           \
                           RESOLVE_VALUEs m35)
#define DEFINE_STRUCT_IMPL_36_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32,                                                      \
                               m33,                                                      \
                               m34,                                                      \
                               m35)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_36(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32),                                \
                                     EXTRs(NTH1OF2s m33),                                \
                                     EXTRs(NTH1OF2s m34),                                \
                                     EXTRs(NTH1OF2s m35));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 32>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m33),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m33)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 33>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m34),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m34)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 34>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m35),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m35)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_37(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32,                                                       \
                              m33,                                                       \
                              m34,                                                       \
                              m35,                                                       \
                              m36)                                                       \
    DEFINE_STRUCT_IMPL_37_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32,                                           \
                           RESOLVE_VALUEs m33,                                           \
                           RESOLVE_VALUEs m34,                                           \
                           RESOLVE_VALUEs m35,                                           \
                           RESOLVE_VALUEs m36)
#define DEFINE_STRUCT_IMPL_37_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32,                                                      \
                               m33,                                                      \
                               m34,                                                      \
                               m35,                                                      \
                               m36)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_37(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32),                                \
                                     EXTRs(NTH1OF2s m33),                                \
                                     EXTRs(NTH1OF2s m34),                                \
                                     EXTRs(NTH1OF2s m35),                                \
                                     EXTRs(NTH1OF2s m36));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 32>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m33),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m33)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 33>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m34),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m34)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 34>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m35),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m35)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 35>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m36),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m36)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT

#define DEFINE_STRUCT_IMPL_38(Type,                                                      \
                              m1,                                                        \
                              m2,                                                        \
                              m3,                                                        \
                              m4,                                                        \
                              m5,                                                        \
                              m6,                                                        \
                              m7,                                                        \
                              m8,                                                        \
                              m9,                                                        \
                              m10,                                                       \
                              m11,                                                       \
                              m12,                                                       \
                              m13,                                                       \
                              m14,                                                       \
                              m15,                                                       \
                              m16,                                                       \
                              m17,                                                       \
                              m18,                                                       \
                              m19,                                                       \
                              m20,                                                       \
                              m21,                                                       \
                              m22,                                                       \
                              m23,                                                       \
                              m24,                                                       \
                              m25,                                                       \
                              m26,                                                       \
                              m27,                                                       \
                              m28,                                                       \
                              m29,                                                       \
                              m30,                                                       \
                              m31,                                                       \
                              m32,                                                       \
                              m33,                                                       \
                              m34,                                                       \
                              m35,                                                       \
                              m36,                                                       \
                              m37)                                                       \
    DEFINE_STRUCT_IMPL_38_(Type,                                                         \
                           RESOLVE_VALUEs m1,                                            \
                           RESOLVE_VALUEs m2,                                            \
                           RESOLVE_VALUEs m3,                                            \
                           RESOLVE_VALUEs m4,                                            \
                           RESOLVE_VALUEs m5,                                            \
                           RESOLVE_VALUEs m6,                                            \
                           RESOLVE_VALUEs m7,                                            \
                           RESOLVE_VALUEs m8,                                            \
                           RESOLVE_VALUEs m9,                                            \
                           RESOLVE_VALUEs m10,                                           \
                           RESOLVE_VALUEs m11,                                           \
                           RESOLVE_VALUEs m12,                                           \
                           RESOLVE_VALUEs m13,                                           \
                           RESOLVE_VALUEs m14,                                           \
                           RESOLVE_VALUEs m15,                                           \
                           RESOLVE_VALUEs m16,                                           \
                           RESOLVE_VALUEs m17,                                           \
                           RESOLVE_VALUEs m18,                                           \
                           RESOLVE_VALUEs m19,                                           \
                           RESOLVE_VALUEs m20,                                           \
                           RESOLVE_VALUEs m21,                                           \
                           RESOLVE_VALUEs m22,                                           \
                           RESOLVE_VALUEs m23,                                           \
                           RESOLVE_VALUEs m24,                                           \
                           RESOLVE_VALUEs m25,                                           \
                           RESOLVE_VALUEs m26,                                           \
                           RESOLVE_VALUEs m27,                                           \
                           RESOLVE_VALUEs m28,                                           \
                           RESOLVE_VALUEs m29,                                           \
                           RESOLVE_VALUEs m30,                                           \
                           RESOLVE_VALUEs m31,                                           \
                           RESOLVE_VALUEs m32,                                           \
                           RESOLVE_VALUEs m33,                                           \
                           RESOLVE_VALUEs m34,                                           \
                           RESOLVE_VALUEs m35,                                           \
                           RESOLVE_VALUEs m36,                                           \
                           RESOLVE_VALUEs m37)
#define DEFINE_STRUCT_IMPL_38_(Type,                                                     \
                               m1,                                                       \
                               m2,                                                       \
                               m3,                                                       \
                               m4,                                                       \
                               m5,                                                       \
                               m6,                                                       \
                               m7,                                                       \
                               m8,                                                       \
                               m9,                                                       \
                               m10,                                                      \
                               m11,                                                      \
                               m12,                                                      \
                               m13,                                                      \
                               m14,                                                      \
                               m15,                                                      \
                               m16,                                                      \
                               m17,                                                      \
                               m18,                                                      \
                               m19,                                                      \
                               m20,                                                      \
                               m21,                                                      \
                               m22,                                                      \
                               m23,                                                      \
                               m24,                                                      \
                               m25,                                                      \
                               m26,                                                      \
                               m27,                                                      \
                               m28,                                                      \
                               m29,                                                      \
                               m30,                                                      \
                               m31,                                                      \
                               m32,                                                      \
                               m33,                                                      \
                               m34,                                                      \
                               m35,                                                      \
                               m36,                                                      \
                               m37)                                                      \
    BOOST_HANA_DEFINE_STRUCT_IMPL_38(Type,                                               \
                                     EXTRs(NTH1OF2s m1),                                 \
                                     EXTRs(NTH1OF2s m2),                                 \
                                     EXTRs(NTH1OF2s m3),                                 \
                                     EXTRs(NTH1OF2s m4),                                 \
                                     EXTRs(NTH1OF2s m5),                                 \
                                     EXTRs(NTH1OF2s m6),                                 \
                                     EXTRs(NTH1OF2s m7),                                 \
                                     EXTRs(NTH1OF2s m8),                                 \
                                     EXTRs(NTH1OF2s m9),                                 \
                                     EXTRs(NTH1OF2s m10),                                \
                                     EXTRs(NTH1OF2s m11),                                \
                                     EXTRs(NTH1OF2s m12),                                \
                                     EXTRs(NTH1OF2s m13),                                \
                                     EXTRs(NTH1OF2s m14),                                \
                                     EXTRs(NTH1OF2s m15),                                \
                                     EXTRs(NTH1OF2s m16),                                \
                                     EXTRs(NTH1OF2s m17),                                \
                                     EXTRs(NTH1OF2s m18),                                \
                                     EXTRs(NTH1OF2s m19),                                \
                                     EXTRs(NTH1OF2s m20),                                \
                                     EXTRs(NTH1OF2s m21),                                \
                                     EXTRs(NTH1OF2s m22),                                \
                                     EXTRs(NTH1OF2s m23),                                \
                                     EXTRs(NTH1OF2s m24),                                \
                                     EXTRs(NTH1OF2s m25),                                \
                                     EXTRs(NTH1OF2s m26),                                \
                                     EXTRs(NTH1OF2s m27),                                \
                                     EXTRs(NTH1OF2s m28),                                \
                                     EXTRs(NTH1OF2s m29),                                \
                                     EXTRs(NTH1OF2s m30),                                \
                                     EXTRs(NTH1OF2s m31),                                \
                                     EXTRs(NTH1OF2s m32),                                \
                                     EXTRs(NTH1OF2s m33),                                \
                                     EXTRs(NTH1OF2s m34),                                \
                                     EXTRs(NTH1OF2s m35),                                \
                                     EXTRs(NTH1OF2s m36),                                \
                                     EXTRs(NTH1OF2s m37));                               \
    static auto metadata() {                                                             \
        return boost::hana::make_tuple(                                                  \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 0>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m1),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m1)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 1>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m2),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m2)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 2>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m3),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m3)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 3>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m4),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m4)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 4>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m5),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m5)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 5>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m6),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m6)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 6>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m7),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m7)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 7>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m8),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m8)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 8>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m9),                     \
                                        EVALs(NTH2OF2s NTH2OF2s m9)),                    \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 9>(),            \
                                        EVALs(NTH1OF2s NTH2OF2s m10),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m10)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 10>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m11),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m11)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 11>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m12),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m12)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 12>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m13),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m13)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 13>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m14),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m14)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 14>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m15),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m15)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 15>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m16),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m16)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 16>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m17),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m17)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 17>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m18),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m18)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 18>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m19),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m19)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 19>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m20),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m20)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 20>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m21),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m21)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 21>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m22),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m22)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 22>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m23),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m23)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 23>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m24),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m24)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 24>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m25),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m25)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 25>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m26),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m26)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 26>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m27),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m27)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 27>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m28),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m28)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 28>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m29),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m29)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 29>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m30),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m30)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 30>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m31),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m31)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 31>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m32),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m32)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 32>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m33),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m33)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 33>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m34),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m34)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 34>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m35),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m35)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 35>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m36),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m36)),                   \
                boost::hana::make_tuple(yenxo::detail::memberName<Type, 36>(),           \
                                        EVALs(NTH1OF2s NTH2OF2s m37),                    \
                                        EVALs(NTH2OF2s NTH2OF2s m37)));                  \
    }                                                                                    \
    static auto const& defaults() {                                                      \
        static auto const ret = yenxo::filterDefaults(metadata());                       \
        return ret;                                                                      \
    }                                                                                    \
    static auto const& names() {                                                         \
        static auto const ret = yenxo::filterNames(metadata());                          \
        return ret;                                                                      \
    }                                                                                    \
    struct ANONYMOUS_STRUCT
