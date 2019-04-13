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


// local
#include <serialize/type_name.hpp>

// 3rd
#include <boost/hana/detail/preprocessor.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>


namespace serialize {


class BadEnumValue : std::runtime_error {
public:
    template <class E>
    explicit BadEnumValue(E v)
        : std::runtime_error(
              "'" + std::to_string(std::underlying_type_t<E>(v)) + "'" +
              " is not a " +
              "'" + std::string(qualifiedTypeName<std::decay_t<E>>()) + "'" +
              " value")
    {}
};


}


/// Is macros argument empty
/// Examples:
/// 	ISEMPTY() -> 1
/// 	ISEMPTY(1) -> 0
/// 	ISEMPTY(,) -> 0
#define ISEMPTY(...) \
ISEMPTY_( \
          /* test if there is just one argument, eventually an empty
             one */ \
          HAS_COMMA(__VA_ARGS__), \
          /* test if TRIGGER_PARENTHESIS_ together with the argument
             adds a comma */ \
          HAS_COMMA(TRIGGER_PARENTHESIS_ __VA_ARGS__), \
          /* test if the argument together with a parenthesis
             adds a comma */ \
          HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
          /* test if placing it between TRIGGER_PARENTHESIS_ and the
             parenthesis adds a comma */ \
          HAS_COMMA(TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
          )

/// Determines if macros argument is a tuple
/// Examples:
/// 	IS_TUPLE() -> 0
/// 	IS_TUPLE(1) -> 0
/// 	IS_TUPLE((1)) -> 1
#define IS_TUPLE(x) \
    HAS_COMMA(TRIGGER_PARENTHESIS_ x)

#define ARG16_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) ARG16_(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0)
#define TRIGGER_PARENTHESIS_(...) ,

#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define ISEMPTY_(_0, _1, _2, _3) HAS_COMMA(PASTE5(IS_EMPTY_CASE_, _0, _1, _2, _3))
#define IS_EMPTY_CASE_0001 ,


/// Enum init
/// Given a tuple (x) || (x,) || (x,1,) || (x,1,"x") || (x,,"x") || (x,,)
/// produces       x  ||  x   ||  x = 1 ||  x = 1    || x        || x
#define EQe(...) EQe_IMPL(BOOST_HANA_PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define EQe_IMPL(N, ...) BOOST_HANA_PP_CONCAT(EQe_IMPL_, N)(__VA_ARGS__)
#define EQe_IMPL_1(x) x
#define EQe_IMPL_2(x, y) EQe_IMPL_2_EMPTY(ISEMPTY(y), x, y)
#define EQe_IMPL_2_EMPTY(_01, x, y) BOOST_HANA_PP_CONCAT(EQe_IMPL_2_EMPTY_, _01)(x, y)
#define EQe_IMPL_2_EMPTY_0(x, y) x = y
#define EQe_IMPL_2_EMPTY_1(x, y) x
#define EQe_IMPL_3(x, y, ...) EQe_IMPL_2_EMPTY(ISEMPTY(y), x, y)


/// Enum string representation
/// Given a tuple (x) || (x,) || (x,1,) || (x,1,"x") || (x,,"x") || (x,,)
/// produces      #x  || #x   || #x     || "x"       || "x"      || #x
#define STRe(...) STRe_IMPL(BOOST_HANA_PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define STRe_IMPL(N, ...) BOOST_HANA_PP_CONCAT(STRe_IMPL_, N)(__VA_ARGS__)
#define STRe_IMPL_1(x) #x
#define STRe_IMPL_2(x, ...) #x
#define STRe_IMPL_3(y, z, x) STRe_IMPL_3_EMPTY(ISEMPTY(x), y, x)
#define STRe_IMPL_3_EMPTY(_01, x, y) BOOST_HANA_PP_CONCAT(STRe_IMPL_3_EMPTY_, _01)(x, y)
#define STRe_IMPL_3_EMPTY_0(x, y) y
#define STRe_IMPL_3_EMPTY_1(x, y) #x


/// Resolve a value entry argument
/// If argument is a tuple produces: (x, EQe x, STRe x) else (x, x, #x)
#define RESOLVE_VALUE(e) \
    RESOLVE_VALUE_IMPL(IS_TUPLE(e), e)
#define RESOLVE_VALUE_IMPL(_01, e) \
    BOOST_HANA_PP_CONCAT(RESOLVE_VALUE_IMPL_, _01)(e)
#define RESOLVE_VALUE_IMPL_0(e) (e, e, #e)  // not tuple
#define RESOLVE_VALUE_IMPL_1(e) (BOOST_HANA_PP_FRONT e, EQe e, STRe e)  // tuple


/// 3tuple argument extraction
#define FIRST3(x, y, z) x
#define SECOND3(x, y, z) y
#define THIRD3(x, y, z) z


/// Generates an enum and traits structure for it
///
/// Example:
/// 	DEFINE_ENUM(MyEnum,
/// 		val0,
/// 		(val1),
/// 		(val2, 4),
/// 		(val3,, "string_repr"),
/// 		(val4, 9, "string_repr2")
/// 	);
#define DEFINE_ENUM(Type, ...) \
    DEFINE_ENUM_IMPL(BOOST_HANA_PP_NARG(__VA_ARGS__), Type, __VA_ARGS__)

#define DEFINE_ENUM_IMPL(N, Type, ...) \
    BOOST_HANA_PP_CONCAT(DEFINE_ENUM_IMPL_, N)(Type, __VA_ARGS__)

#define DEFINE_ENUM_IMPL_1(Type, e1) DEFINE_ENUM_IMPL_1_(Type, RESOLVE_VALUE(e1))
#define DEFINE_ENUM_IMPL_1_(Type, e1) \
    enum class [[maybe_unused]] Type { SECOND3 e1 }; \
    struct Type##Traits; \
    [[maybe_unused]] Type##Traits traits(Type); \
    struct [[maybe_unused]] Type##Traits { \
        using Enum [[maybe_unused]] = Type; \
        [[maybe_unused]] static constexpr size_t count = 1; \
        [[maybe_unused]] static constexpr std::array<Enum, count> values{ \
            Enum::FIRST3 e1  \
        }; \
        [[maybe_unused]] static char const* toString(Enum x) { \
            switch (x) { \
            case Enum::FIRST3 e1: return THIRD3 e1; \
            } \
            assert(false); \
            throw serialize::BadEnumValue(x); \
        } \
        friend Type##Traits traits(Type) { return {}; } \
    }

#define DEFINE_ENUM_IMPL_2(Type, e1, e2) DEFINE_ENUM_IMPL_2_(Type, RESOLVE_VALUE(e1), RESOLVE_VALUE(e2))
#define DEFINE_ENUM_IMPL_2_(Type, e1, e2) \
    enum class [[maybe_unused]] Type { SECOND3 e1, SECOND3 e2 }; \
    struct Type##Traits; \
    [[maybe_unused]] Type##Traits traits(Type); \
    struct [[maybe_unused]] Type##Traits { \
        using Enum [[maybe_unused]] = Type; \
        [[maybe_unused]] static constexpr size_t count = 2; \
        [[maybe_unused]] static constexpr std::array<Enum, count> values{ \
            Enum::FIRST3 e1, Enum::FIRST3 e2  \
        }; \
        [[maybe_unused]] static char const* toString(Enum x) { \
            switch (x) { \
            case Enum::FIRST3 e1: return THIRD3 e1; \
            case Enum::FIRST3 e2: return THIRD3 e2; \
            } \
            assert(false); \
            throw serialize::BadEnumValue(x); \
        } \
        friend Type##Traits traits(Type) { return {}; } \
    }

#define DEFINE_ENUM_IMPL_3(Type, e1, e2, e3) DEFINE_ENUM_IMPL_3_(Type, RESOLVE_VALUE(e1), RESOLVE_VALUE(e2), RESOLVE_VALUE(e3))
#define DEFINE_ENUM_IMPL_3_(Type, e1, e2, e3) \
    enum class [[maybe_unused]] Type { SECOND3 e1, SECOND3 e2, SECOND3 e3 }; \
    struct Type##Traits; \
    [[maybe_unused]] Type##Traits traits(Type); \
    struct [[maybe_unused]] Type##Traits { \
        using Enum [[maybe_unused]] = Type; \
        [[maybe_unused]] static constexpr size_t count = 3; \
        [[maybe_unused]] static constexpr std::array<Enum, count> values{ \
            Enum::FIRST3 e1, Enum::FIRST3 e2, Enum::FIRST3 e3  \
        }; \
        [[maybe_unused]] static char const* toString(Enum x) { \
            switch (x) { \
            case Enum::FIRST3 e1: return THIRD3 e1; \
            case Enum::FIRST3 e2: return THIRD3 e2; \
            case Enum::FIRST3 e3: return THIRD3 e3; \
            } \
            assert(false); \
            throw serialize::BadEnumValue(x); \
        } \
        friend Type##Traits traits(Type) { return {}; } \
    }
