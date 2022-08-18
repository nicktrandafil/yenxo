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

#include <yenxo/exception.hpp>
#include <yenxo/meta.hpp>
#include <yenxo/type_name.hpp>
#include <yenxo/variant.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <cmath>
#include <ostream>
#include <typeinfo>

namespace yenxo {

Variant::~Variant() noexcept {
    switch (type_tag_) {
    case TypeTag::string:
        delete reinterpret_cast<std::string*>(value_.ptr);
        break;
    case TypeTag::vec:
        delete reinterpret_cast<Vec*>(value_.ptr);
        break;
    case TypeTag::map:
        delete reinterpret_cast<Map*>(value_.ptr);
        break;
    default:
        break;
    }
}

Variant::Variant() noexcept
        : type_tag_(TypeTag::null)
        , value_(NullType{}) {
}
Variant::Variant(NullType x) noexcept
        : type_tag_(TypeTag::null)
        , value_(x) {
}
Variant::Variant(bool x) noexcept
        : type_tag_(TypeTag::boolean)
        , value_(x) {
}
Variant::Variant(char x) noexcept
        : type_tag_(TypeTag::char_)
        , value_(x) {
}
Variant::Variant(int8_t x) noexcept
        : type_tag_(TypeTag::int8)
        , value_(x) {
}
Variant::Variant(uint8_t x) noexcept
        : type_tag_(TypeTag::uint8)
        , value_(x) {
}
Variant::Variant(int16_t x) noexcept
        : type_tag_(TypeTag::int16)
        , value_(x) {
}
Variant::Variant(uint16_t x) noexcept
        : type_tag_(TypeTag::uint16)
        , value_(x) {
}
Variant::Variant(int32_t x) noexcept
        : type_tag_(TypeTag::int32)
        , value_(x) {
}
Variant::Variant(uint32_t x) noexcept
        : type_tag_(TypeTag::uint32)
        , value_(x) {
}
Variant::Variant(int64_t x) noexcept
        : type_tag_(TypeTag::int64)
        , value_(x) {
}
Variant::Variant(uint64_t x) noexcept
        : type_tag_(TypeTag::uint64)
        , value_(x) {
}
Variant::Variant(double x) noexcept
        : type_tag_(TypeTag::double_)
        , value_(x) {
}

Variant::Variant(char const* const& x)
        : type_tag_(TypeTag::string)
        , value_(new std::string(x)) {
}

Variant::Variant(std::string const& x)
        : type_tag_(TypeTag::string)
        , value_(new std::string(x)) {
}
Variant::Variant(std::string&& x)
        : type_tag_(TypeTag::string)
        , value_(new std::string(std::move(x))) {
}
Variant::Variant(std::string_view x)
        : type_tag_(TypeTag::string)
        , value_(new std::string(x)) {
}

Variant::Variant(Vec const& x)
        : type_tag_(TypeTag::vec)
        , value_(new Vec(x)) {
}
Variant::Variant(Vec&& x)
        : type_tag_(TypeTag::vec)
        , value_(new Vec(std::move(x))) {
}

Variant::Variant(Map const& x)
        : type_tag_(TypeTag::map)
        , value_(new Map(x)) {
}
Variant::Variant(Map&& x)
        : type_tag_(TypeTag::map)
        , value_(new Map(std::move(x))) {
}

struct Variant::Impl {
    static inline ValueType copy(TypeTag tag, ValueType x) {
        ValueType ret;
        switch (tag) {
        case TypeTag::string:
            ret = new std::string(*reinterpret_cast<std::string*>(x.ptr));
            break;
        case TypeTag::vec:
            ret = new Vec(*reinterpret_cast<Vec*>(x.ptr));
            break;
        case TypeTag::map:
            ret = new Map(*reinterpret_cast<Map*>(x.ptr));
            break;
        default:
            ret = x;
            break;
        }
        return ret;
    }

    struct ToJson;
};

Variant::Variant(Variant const& rhs)
        : type_tag_(rhs.type_tag_)
        , value_(Impl::copy(type_tag_, rhs.value_)) {
}

Variant& Variant::operator=(Variant const& rhs) {
    Variant tmp(rhs);
    operator=(std::move(tmp));
    return *this;
}

Variant::Variant(Variant&& rhs) noexcept
        : type_tag_(rhs.type_tag_)
        , value_(rhs.value_) {
    rhs.type_tag_ = TypeTag::null;
    rhs.value_.null_ = {};
}

Variant& Variant::operator=(Variant&& rhs) noexcept {
    this->~Variant();
    new (this) Variant(std::move(rhs));
    return *this;
}

namespace {

template <typename T, typename = void>
struct GetHelper final : GetHelper<T, When<true>> {};

template <typename T, bool condition>
struct GetHelper<T, When<condition>> {
    static T const& apply(T const& x) noexcept {
        return x;
    }
    [[noreturn]] static T const& apply(Variant::NullType) {
        throw VariantEmpty(boost::hana::type_c<T>);
    }
    template <typename U>
    [[noreturn]] static T const& apply(U const&) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<U>);
    }
};

template <>
struct GetHelper<Variant::NullType> {
    static Variant::NullType apply(Variant::NullType) noexcept {
        return {};
    }
    template <typename U>
    [[noreturn]] static Variant::NullType apply(U const&) {
        throw VariantBadType(boost::hana::type_c<Variant::NullType>,
                             boost::hana::type_c<U>);
    }
};

template <typename A, typename B>
constexpr auto same_sign_v = std::is_signed_v<A> == std::is_signed_v<B>;

struct ThrowVariantIntegralOverflow {
    template <class Type, class Value>
    [[noreturn]] static typename Type::type overflow(Type t, Value v) {
        throw VariantIntegralOverflow(std::string(typeName(t)), std::to_string(v));
    }
    template <class Type1, class Type2>
    [[noreturn]] static typename Type1::type badType(Type1 t1, Type2 t2) {
        throw VariantBadType(t1, t2);
    }
};

// Casts `U` to `T` if a specific value of `U` is representable in `T` else throws
// `VariantIntegralOverflow`.
template <typename T,
          typename U,
          typename ThrowPolicy = ThrowVariantIntegralOverflow,
          typename = void>
struct ArithmeticCheckedCast final
        : ArithmeticCheckedCast<T, U, ThrowPolicy, When<true>> {
    static_assert(std::is_arithmetic_v<T> && (std::is_arithmetic_v<U>));
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        same_sign_v<T, U> && sizeof(T) >= sizeof(U)>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        same_sign_v<T, U> && sizeof(T) < sizeof(U)>> {
    static T apply(U x) {
        if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max()) {
            return P::overflow(boost::hana::type_c<T>, x);
        } else {
            return static_cast<T>(x);
        }
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) > sizeof(U))>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) <= sizeof(U))>> {
    static T apply(U x) {
#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
        if (x > std::numeric_limits<T>::max()) {
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif
            return P::overflow(boost::hana::type_c<T>, x);
        }
        return static_cast<T>(x);
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_unsigned_v<T> && std::is_signed_v<U> && sizeof(T) >= sizeof(U)>> {
    static T apply(U x) {
        if (x < 0) {
            return P::overflow(boost::hana::type_c<T>, x);
        } else {
            return static_cast<T>(x);
        }
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_unsigned_v<T> && std::is_signed_v<U> && sizeof(T) < sizeof(U)>> {
    static T apply(U x) {
#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
        if (x < 0 || x > std::numeric_limits<T>::max()) {
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif
            return P::overflow(boost::hana::type_c<T>, x);
        }
        return static_cast<T>(x);
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        std::is_same_v<T, double> &&
        !std::is_same_v<U, bool>>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        std::is_same_v<U, double>>> {
    static T apply(U x) {
        double iptr;
        if (std::modf(x, &iptr) == 0.0) {
            return ArithmeticCheckedCast<T, int64_t, P>::apply(static_cast<int64_t>(x));
        } else {
            return P::overflow(boost::hana::type_c<T>, x);
        }
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        std::is_same_v<T, bool> ^ std::is_same_v<U, bool>>> {
    static T apply(U) {
        return P::badType(boost::hana::type_c<T>, boost::hana::type_c<U>);
    }
};

template <typename T, typename U, typename P>
struct ArithmeticCheckedCast<T, U, P, When<
        std::is_same_v<T, bool> &&
        std::is_same_v<U, bool>>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename Dst>
struct ArithmeticCheckedCastT {
    template <typename Src>
    Dst operator()(Src x) const
            noexcept(noexcept(ArithmeticCheckedCast<Dst, Src>::apply(x))) {
        return ArithmeticCheckedCast<Dst, Src>::apply(x);
    }
};

template <typename T>
constexpr ArithmeticCheckedCastT<T> arithmeticCheckedCast;

template <typename T>
struct GetHelper<T, When<std::is_arithmetic_v<T>>> {
    [[noreturn]] static T apply(Variant::NullType) {
        throw VariantEmpty(boost::hana::type_c<T>);
    }
    static T apply(bool x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(char x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(int8_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(uint8_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(int16_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(uint16_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(int32_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(uint32_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(int64_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(uint64_t x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    static T apply(double x) noexcept(noexcept(arithmeticCheckedCast<T>(x))) {
        return arithmeticCheckedCast<T>(x);
    }
    [[noreturn]] static T apply(std::string const&) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<std::string>);
    }
    [[noreturn]] static T apply(Variant::Vec const&) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<Variant::Vec>);
    }
    [[noreturn]] static T apply(Variant::Map const&) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<Variant::Map>);
    }
};

template <typename T>
struct GetHelper<T, When<std::is_reference_v<T>>> {
    static T apply(T x) noexcept {
        return x;
    }
    [[noreturn]] static T apply(Variant::NullType) {
        throw VariantEmpty(boost::hana::type_c<std::remove_reference_t<T>>);
    }
    template <typename U>
    [[noreturn]] static T apply(U const&) {
        throw VariantBadType(boost::hana::type_c<std::remove_reference_t<T>>,
                             boost::hana::type_c<U>);
    }
};

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type" // safe comparation
template <class T, class U>
decltype(auto) getHelper(Variant::TypeTag tag, U value_) {
    using TypeTag = Variant::TypeTag;
    switch (tag) {
    case TypeTag::null:
        return GetHelper<T>::apply(value_.null_);
    case TypeTag::boolean:
        return GetHelper<T>::apply(value_.bool_);
    case TypeTag::char_:
        return GetHelper<T>::apply(value_.char_);
    case TypeTag::int8:
        return GetHelper<T>::apply(value_.int8);
    case TypeTag::uint8:
        return GetHelper<T>::apply(value_.uint8);
    case TypeTag::int16:
        return GetHelper<T>::apply(value_.int16);
    case TypeTag::uint16:
        return GetHelper<T>::apply(value_.uint16);
    case TypeTag::int32:
        return GetHelper<T>::apply(value_.int32);
    case TypeTag::uint32:
        return GetHelper<T>::apply(value_.uint32);
    case TypeTag::int64:
        return GetHelper<T>::apply(value_.int64);
    case TypeTag::uint64:
        return GetHelper<T>::apply(value_.uint64);
    case TypeTag::double_:
        return GetHelper<T>::apply(value_.double_);
    case TypeTag::string:
        return GetHelper<T>::apply(*reinterpret_cast<std::string*>(value_.ptr));
    case TypeTag::vec:
        return GetHelper<T>::apply(*reinterpret_cast<Variant::Vec*>(value_.ptr));
    case TypeTag::map:
        return GetHelper<T>::apply(*reinterpret_cast<Variant::Map*>(value_.ptr));
    }
}
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

} // namespace

#define GET_HELPER(T, tag, value, x)                                                     \
    if ((tag) == TypeTag::null) {                                                        \
        return x;                                                                        \
    } else {                                                                             \
        return getHelper<T>(tag, value);                                                 \
    }                                                                                    \
    (void)x

Variant::operator Variant::NullType() const {
    return getHelper<NullType>(type_tag_, value_);
}

char Variant::character() const {
    return getHelper<char>(type_tag_, value_);
}

char Variant::characterOr(char x) const {
    GET_HELPER(char, type_tag_, value_, x);
}

int8_t Variant::int8() const {
    return getHelper<int8_t>(type_tag_, value_);
}

int8_t Variant::int8Or(int8_t x) const {
    GET_HELPER(int8_t, type_tag_, value_, x);
}

uint8_t Variant::uint8() const {
    return getHelper<uint8_t>(type_tag_, value_);
}

uint8_t Variant::uint8Or(uint8_t x) const {
    GET_HELPER(uint8_t, type_tag_, value_, x);
}

int16_t Variant::int16() const {
    return getHelper<int16_t>(type_tag_, value_);
}

int16_t Variant::int16Or(int16_t x) const {
    GET_HELPER(int16_t, type_tag_, value_, x);
}

uint16_t Variant::uint16() const {
    return getHelper<uint16_t>(type_tag_, value_);
}

uint16_t Variant::uint16Or(uint16_t x) const {
    GET_HELPER(uint16_t, type_tag_, value_, x);
}

bool Variant::boolean() const {
    return getHelper<bool>(type_tag_, value_);
}

bool Variant::booleanOr(bool x) const {
    GET_HELPER(bool, type_tag_, value_, x);
}

int32_t Variant::int32() const {
    return getHelper<int32_t>(type_tag_, value_);
}

int32_t Variant::int32Or(int32_t x) const {
    GET_HELPER(int32_t, type_tag_, value_, x);
}

uint32_t Variant::uint32() const {
    return getHelper<uint32_t>(type_tag_, value_);
}

uint32_t Variant::uint32Or(uint32_t x) const {
    GET_HELPER(uint32_t, type_tag_, value_, x);
}

int64_t Variant::int64() const {
    return getHelper<int64_t>(type_tag_, value_);
}

int64_t Variant::int64Or(int64_t x) const {
    GET_HELPER(int64_t, type_tag_, value_, x);
}

uint64_t Variant::uint64() const {
    return getHelper<uint64_t>(type_tag_, value_);
}

uint64_t Variant::uint64Or(uint64_t x) const {
    GET_HELPER(uint64_t, type_tag_, value_, x);
}

double Variant::floating() const {
    return getHelper<double>(type_tag_, value_);
}

double Variant::floatingOr(double x) const {
    GET_HELPER(double, type_tag_, value_, x);
}

std::string const& Variant::str() const {
    return getHelper<std::string>(type_tag_, value_);
}

std::string Variant::strOr(std::string const& x) const {
    GET_HELPER(std::string, type_tag_, value_, x);
}

Variant::Vec const& Variant::vec() const {
    return getHelper<Vec>(type_tag_, value_);
}

Variant::Vec Variant::vecOr(Vec const& x) const {
    GET_HELPER(Vec, type_tag_, value_, x);
}

Variant::Vec& Variant::modifyVec() {
    return getHelper<Vec&>(type_tag_, value_);
}

Variant::Map const& Variant::map() const {
    return getHelper<Map>(type_tag_, value_);
}

Variant::Map Variant::mapOr(Map const& x) const {
    GET_HELPER(Map, type_tag_, value_, x);
}

Variant::Map& Variant::modifyMap() {
    return getHelper<Map&>(type_tag_, value_);
}

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal" // safe comparation
bool Variant::operator==(Variant const& rhs) const noexcept {
    if (type_tag_ != rhs.type_tag_) {
        return false;
    } else {
        switch (type_tag_) {
        case TypeTag::null:
            return true;
        case TypeTag::boolean:
            return value_.bool_ == rhs.value_.bool_;
        case TypeTag::char_:
            return value_.char_ == rhs.value_.char_;
        case TypeTag::int8:
            return value_.int8 == rhs.value_.int8;
        case TypeTag::uint8:
            return value_.uint8 == rhs.value_.uint8;
        case TypeTag::int16:
            return value_.int16 == rhs.value_.int16;
        case TypeTag::uint16:
            return value_.uint16 == rhs.value_.uint16;
        case TypeTag::int32:
            return value_.int32 == rhs.value_.int32;
        case TypeTag::uint32:
            return value_.uint32 == rhs.value_.uint32;
        case TypeTag::int64:
            return value_.int64 == rhs.value_.int64;
        case TypeTag::uint64:
            return value_.uint64 == rhs.value_.uint64;
        case TypeTag::double_:
            return value_.double_ == rhs.value_.double_;
        case TypeTag::string:
            return *reinterpret_cast<std::string*>(value_.ptr)
                == *reinterpret_cast<std::string*>(rhs.value_.ptr);
        case TypeTag::vec:
            return *reinterpret_cast<Variant::Vec*>(value_.ptr)
                == *reinterpret_cast<Variant::Vec*>(rhs.value_.ptr);
        case TypeTag::map:
            return *reinterpret_cast<Variant::Map*>(value_.ptr)
                == *reinterpret_cast<Variant::Map*>(rhs.value_.ptr);
        }
        return false;
    }
}
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

namespace {

struct ArithmeticCheckedEqT {
    template <typename Lhs, typename Rhs>
    bool operator()(Lhs lhs, Rhs rhs) const noexcept;
};

constexpr ArithmeticCheckedEqT arithmeticCheckedEq;

template <typename T, typename U, typename = void>
struct ArithmeticCheckedEq final : ArithmeticCheckedEq<T, U, When<true>> {
    static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>);
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        same_sign_v<T, U> && sizeof(T) >= sizeof(U)>> {
    static bool apply(T lhs, U rhs) noexcept {
        return lhs == rhs;
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        same_sign_v<T, U> && sizeof(T) < sizeof(U)>> {
    static bool apply(T lhs, U rhs) {
        return lhs == rhs;
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) > sizeof(U))>> {
    static bool apply(T lhs, U rhs) noexcept {
        return lhs == rhs;
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) <= sizeof(U))>> {
    static bool apply(T lhs, U rhs) {
        if (lhs < 0) {
            return false;
        } else {
            return static_cast<U>(lhs) == rhs;
        }
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_unsigned_v<T> && std::is_signed_v<U> && sizeof(T) >= sizeof(U)>> {
    static bool apply(T lhs, U rhs) {
        return arithmeticCheckedEq(rhs, lhs);
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        !std::is_same_v<U, double> &&
        !std::is_same_v<U, bool> &&
        std::is_unsigned_v<T> && std::is_signed_v<U> && sizeof(T) < sizeof(U)>> {
    static bool apply(T lhs, U rhs) {
        return lhs == rhs;
    }
};

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        std::is_same_v<T, double> &&
        !std::is_same_v<U, bool>>> {
    static bool apply(T lhs, U rhs) {
        return lhs == rhs;
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        !std::is_same_v<T, double> &&
        !std::is_same_v<T, bool> &&
        std::is_same_v<U, double>>> {
    static bool apply(T lhs, U rhs) {
        return lhs == rhs;
    }
};
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        std::is_same_v<T, bool> ^ std::is_same_v<U, bool>>> {
    static bool apply(T, U) {
        return false;
    }
};

template <typename T, typename U>
struct ArithmeticCheckedEq<T, U, When<
        std::is_same_v<T, bool> &&
        std::is_same_v<U, bool>>> {
    static bool apply(T lhs, U rhs) {
        return lhs == rhs;
    }
};

template <typename Lhs, typename Rhs>
bool ArithmeticCheckedEqT::operator()(Lhs lhs, Rhs rhs) const noexcept {
    return ArithmeticCheckedEq<Lhs, Rhs>::apply(lhs, rhs);
}

template <class T, class U>
bool equalArithmetic(T lhs, Variant::TypeTag tag, U const& rhs) {
    using TypeTag = Variant::TypeTag;
    static_assert(std::is_arithmetic_v<T>);
    switch (tag) {
    case TypeTag::null:
        return false;
    case TypeTag::boolean:
        return arithmeticCheckedEq(lhs, rhs.bool_);
    case TypeTag::char_:
        return arithmeticCheckedEq(lhs, rhs.char_);
    case TypeTag::int8:
        return arithmeticCheckedEq(lhs, rhs.int8);
    case TypeTag::uint8:
        return arithmeticCheckedEq(lhs, rhs.uint8);
    case TypeTag::int16:
        return arithmeticCheckedEq(lhs, rhs.int16);
    case TypeTag::uint16:
        return arithmeticCheckedEq(lhs, rhs.uint16);
    case TypeTag::int32:
        return arithmeticCheckedEq(lhs, rhs.int32);
    case TypeTag::uint32:
        return arithmeticCheckedEq(lhs, rhs.uint32);
    case TypeTag::int64:
        return arithmeticCheckedEq(lhs, rhs.int64);
    case TypeTag::uint64:
        return arithmeticCheckedEq(lhs, rhs.uint64);
    case TypeTag::double_:
        return arithmeticCheckedEq(lhs, rhs.double_);
    case TypeTag::string:
        return false;
    case TypeTag::vec:
        return false;
    case TypeTag::map:
        return false;
    }
    assert(false);
    return false;
}

} // namespace

bool equal(Variant const& lhs, Variant const& rhs) {
    using TypeTag = Variant::TypeTag;
    using Map = Variant::Map;

    switch (lhs.type_tag_) {
    case TypeTag::null:
        return rhs.null();

    case TypeTag::boolean:
        return equalArithmetic(lhs.value_.bool_, rhs.type_tag_, rhs.value_);
    case TypeTag::char_:
        return equalArithmetic(lhs.value_.char_, rhs.type_tag_, rhs.value_);
    case TypeTag::int8:
        return equalArithmetic(lhs.value_.int8, rhs.type_tag_, rhs.value_);
    case TypeTag::uint8:
        return equalArithmetic(lhs.value_.uint8, rhs.type_tag_, rhs.value_);
    case TypeTag::int16:
        return equalArithmetic(lhs.value_.int16, rhs.type_tag_, rhs.value_);
    case TypeTag::uint16:
        return equalArithmetic(lhs.value_.uint16, rhs.type_tag_, rhs.value_);
    case TypeTag::int32:
        return equalArithmetic(lhs.value_.int32, rhs.type_tag_, rhs.value_);
    case TypeTag::uint32:
        return equalArithmetic(lhs.value_.uint32, rhs.type_tag_, rhs.value_);
    case TypeTag::int64:
        return equalArithmetic(lhs.value_.int64, rhs.type_tag_, rhs.value_);
    case TypeTag::uint64:
        return equalArithmetic(lhs.value_.uint64, rhs.type_tag_, rhs.value_);
    case TypeTag::double_:
        return equalArithmetic(lhs.value_.double_, rhs.type_tag_, rhs.value_);

    case TypeTag::string:
        return lhs == rhs;
    case TypeTag::vec: {
        if (TypeTag::vec != rhs.type()) {
            return false;
        }
        auto const& lhs_vec = lhs.vec();
        auto const& rhs_vec = rhs.vec();
        return lhs_vec.size() == rhs_vec.size()
            && std::equal(lhs_vec.begin(), lhs_vec.end(), rhs_vec.begin(), &equal);
    }
    case TypeTag::map: {
        if (TypeTag::map != rhs.type()) {
            return false;
        }
        auto const& lhs_map = lhs.map();
        auto const& rhs_map = rhs.map();
        if (lhs_map.size() != rhs_map.size()) {
            return false;
        }
        std::vector<Map::const_pointer> lhs(lhs_map.size());
        std::vector<Map::const_pointer> rhs(rhs_map.size());
        std::transform(lhs_map.begin(), lhs_map.end(), lhs.begin(), [](auto const& x) {
            return std::addressof(x);
        });
        std::transform(rhs_map.begin(), rhs_map.end(), rhs.begin(), [](auto const& x) {
            return std::addressof(x);
        });
        std::sort(lhs.begin(), lhs.end(), [](auto lhs, auto rhs) {
            return lhs->first < rhs->first;
        });
        std::sort(rhs.begin(), rhs.end(), [](auto lhs, auto rhs) {
            return lhs->first < rhs->first;
        });
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](auto lhs, auto rhs) {
            return lhs->first == rhs->first && equal(lhs->second, rhs->second);
        });
    }
    }
    return false;
}

bool Variant::operator!=(Variant const& rhs) const noexcept {
    return !this->operator==(rhs);
}

namespace {

using namespace rapidjson;

/// RapidJSON visitor
template <typename Encoding>
struct FromJson : rapidjson::BaseReaderHandler<Encoding, FromJson<Encoding>> {
    template <class T>
    void val(T&& x) {
        switch (ptrs.back()->type()) {
        case Variant::TypeTag::map:
            ptrs.back()->modifyMap()[std::move(key)] = Variant(std::forward<T>(x));
            break;
        case Variant::TypeTag::vec:
            ptrs.back()->modifyVec().push_back(Variant(std::forward<T>(x)));
            break;
        default:
            *ptrs.back() = Variant(std::forward<T>(x));
        }
    }

    template <class T>
    Variant* val2(T&& x) {
        switch (ptrs.back()->type()) {
        case Variant::TypeTag::map:
            return &(ptrs.back()->modifyMap()[std::move(key)] = Variant(std::forward<T>(x)));
        case Variant::TypeTag::vec:
            ptrs.back()->modifyVec().push_back(Variant(std::forward<T>(x)));
            return &ptrs.back()->modifyVec().back();
        default:
            return &(*ptrs.back() = Variant(std::forward<T>(x)));
        }
    }

    bool Null() {
        val(Variant::NullType());
        return true;
    }
    bool Bool(bool b) {
        val(b);
        return true;
    }
    bool Int(int32_t i) {
        val(i);
        return true;
    }
    bool Uint(uint32_t u) {
        val(u);
        return true;
    }
    bool Int64(int64_t i64) {
        val(i64);
        return true;
    }
    bool Uint64(uint64_t u64) {
        val(u64);
        return true;
    }
    bool Double(double d) {
        val(d);
        return true;
    }
    bool String(typename Encoding::Ch const* str, SizeType length, bool) {
        val(std::string(str, length));
        return true;
    }
    bool StartObject() {
        ptrs.push_back(val2(Variant::Map()));
        return true;
    }
    bool Key(typename Encoding::Ch const* str, SizeType length, bool) {
        assert(ptrs.back()->type() == Variant::TypeTag::map);
        key = std::string(str, length);
        return true;
    }
    bool EndObject(SizeType n) {
        assert(ptrs.back()->type() == Variant::TypeTag::map);
        assert(ptrs.back()->map().size() == n);
        (void)n;
        ptrs.pop_back();
        return true;
    }
    bool StartArray() {
        ptrs.push_back(val2(Variant::Vec()));
        return true;
    }
    bool EndArray(SizeType n) {
        assert(ptrs.back()->type() == Variant::TypeTag::vec);
        assert(ptrs.back()->vec().size() == n);
        (void)n;
        ptrs.pop_back();
        return true;
    }

    Variant var;
    std::vector<Variant*> ptrs{&var};
    std::string key;
};

} // namespace

Variant Variant::from(Value const& json) {
    FromJson<Value::EncodingType> ser;
    json.Accept(ser);
    assert(ser.ptrs.size() == 1);
    return std::move(ser).var;
}

Variant Variant::fromJson(std::string const& json) {
    FromJson<rapidjson::UTF8<>> handler;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json.c_str());
    reader.Parse(ss, handler);
    if (reader.HasParseError()) {
        throw std::runtime_error(rapidjson::GetParseError_En(reader.GetParseErrorCode()));
    }
    return std::move(handler).var;
}

struct Variant::Impl::ToJson {
    Variant const& var;

    explicit ToJson(Variant const& var)
            : var(var) {
    }

    template <class Handler>
    bool operator()(Handler& handler) const {
        apply(handler, var);
        return true;
    }

    template <class Handler>
    static void apply(Handler& dst, Variant const& var) {
        Document d;
        switch (var.type_tag_) {
        case TypeTag::null:
            dst.Null();
            break;
        case TypeTag::boolean:
            dst.Bool(var.value_.bool_);
            break;
        case TypeTag::char_:
            dst.Int(var.value_.char_);
            break;
        case TypeTag::int8:
            dst.Int(var.value_.int8);
            break;
        case TypeTag::uint8:
            dst.Uint(var.value_.uint8);
            break;
        case TypeTag::int16:
            dst.Int(var.value_.int16);
            break;
        case TypeTag::uint16:
            dst.Uint(var.value_.uint16);
            break;
        case TypeTag::int32:
            dst.Int(var.value_.int32);
            break;
        case TypeTag::uint32:
            dst.Uint(var.value_.uint32);
            break;
        case TypeTag::int64:
            dst.Int64(var.value_.int64);
            break;
        case TypeTag::uint64:
            dst.Uint64(var.value_.uint64);
            break;
        case TypeTag::double_:
            dst.Double(var.value_.double_);
            break;
        case TypeTag::string: {
            auto const str = reinterpret_cast<std::string*>(var.value_.ptr);
            dst.String(str->c_str(), static_cast<unsigned int>(str->size()), true);
            break;
        }
        case TypeTag::vec: {
            dst.StartArray();
            auto const vec = reinterpret_cast<Variant::Vec*>(var.value_.ptr);
            for (auto const& var : *vec) {
                apply(dst, var);
            }
            dst.EndArray(static_cast<unsigned int>(vec->size()));
            break;
        }
        case TypeTag::map: {
            dst.StartObject();
            auto const map = reinterpret_cast<Variant::Map*>(var.value_.ptr);
            for (auto const& [key, var] : *map) {
                dst.Key(key.c_str(), static_cast<unsigned int>(key.size()), true);
                apply(dst, var);
            }
            dst.EndObject(static_cast<unsigned int>(map->size()));
            break;
        }
        }
    }
};

rapidjson::Document& Variant::to(rapidjson::Document& json) const {
    Impl::ToJson const sax_event_gen(*this);
    json.Populate(sax_event_gen);
    return json;
}

std::string Variant::toJson() const {
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    Impl::ToJson (*this)(writer);
    return sb.GetString();
}

std::string Variant::toPrettyJson() const {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    Impl::ToJson (*this)(writer);
    return sb.GetString();
}

std::ostream& operator<<(std::ostream& os, Variant const& var) {
    using TypeTag = Variant::TypeTag;
    switch (var.type_tag_) {
    case TypeTag::null:
        os << "Null";
        break;
    case TypeTag::boolean:
        os  << var.value_.bool_;
        break;
    case TypeTag::char_:
        os << var.value_.char_;
        break;
    case TypeTag::int8:
        os << var.value_.int8;
        break;
    case TypeTag::uint8:
        os << var.value_.uint8;
        break;
    case TypeTag::int16:
        os << var.value_.int16;
        break;
    case TypeTag::uint16:
        os << var.value_.uint16;
        break;
    case TypeTag::int32:
        os << var.value_.int32;
        break;
    case TypeTag::uint32:
        os << var.value_.uint32;
        break;
    case TypeTag::int64:
        os << var.value_.int64;
        break;
    case TypeTag::uint64:
        os << var.value_.uint64;
        break;
    case TypeTag::double_:
        os << var.value_.double_;
        break;
    case TypeTag::string:
        os << *reinterpret_cast<std::string*>(var.value_.ptr);
        break;
    case TypeTag::vec: {
        auto const vec = reinterpret_cast<Variant::Vec*>(var.value_.ptr);
        auto const l = vec->size() - 1;
        std::size_t i = 0;
        os << "[ ";
        for (auto const& x : *vec) {
            os << x << ((i++ == l) ? " " : ", ");
        }
        os << "]";
        break;
    }
    case TypeTag::map: {
        auto const map = reinterpret_cast<Variant::Map*>(var.value_.ptr);
        os << "{ ";
        for (auto const& [key, x] : *map) {
            os << key << ": " << x << "; ";
        }
        os << "}";
        break;
    }
    }
    return os;
}

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type" // safe comparation
std::type_info const& Variant::typeInfo() const noexcept {
    switch (type_tag_) {
    case TypeTag::null:
        return typeid(NullType);
    case TypeTag::boolean:
        return typeid(bool);
    case TypeTag::char_:
        return typeid(char);
    case TypeTag::int8:
        return typeid(int8_t);
    case TypeTag::uint8:
        return typeid(uint8_t);
    case TypeTag::int16:
        return typeid(int16_t);
    case TypeTag::uint16:
        return typeid(uint16_t);
    case TypeTag::int32:
        return typeid(int32_t);
    case TypeTag::uint32:
        return typeid(uint32_t);
    case TypeTag::int64:
        return typeid(int64_t);
    case TypeTag::uint64:
        return typeid(uint64_t);
    case TypeTag::double_:
        return typeid(double);
    case TypeTag::string:
        return typeid(std::string);
    case TypeTag::vec:
        return typeid(Vec);
    case TypeTag::map:
        return typeid(Map);
    }
}
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

bool Variant::null() const noexcept {
    return type_tag_ == TypeTag::null;
}

} // namespace yenxo
