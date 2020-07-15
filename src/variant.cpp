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

#include <serialize/exception.hpp>
#include <serialize/meta.hpp>
#include <serialize/type_name.hpp>
#include <serialize/variant.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <ostream>
#include <typeinfo>

namespace serialize {

Variant::~Variant() noexcept {
    switch (type_tag_) {
    case TypeTag::string: delete reinterpret_cast<std::string*>(value_.ptr); break;
    case TypeTag::vec: delete reinterpret_cast<Vec*>(value_.ptr); break;
    case TypeTag::map: delete reinterpret_cast<Map*>(value_.ptr); break;
    default: break;
    }
}

Variant::Variant() noexcept : type_tag_(TypeTag::null), value_(NullType{}) {
}
Variant::Variant(NullType x) noexcept : type_tag_(TypeTag::null), value_(x) {
}
Variant::Variant(bool x) noexcept : type_tag_(TypeTag::boolean), value_(x) {
}
Variant::Variant(char x) noexcept : type_tag_(TypeTag::char_), value_(x) {
}
Variant::Variant(unsigned char x) noexcept : type_tag_(TypeTag::uchar), value_(x) {
}
Variant::Variant(int16_t x) noexcept : type_tag_(TypeTag::int16), value_(x) {
}
Variant::Variant(uint16_t x) noexcept : type_tag_(TypeTag::uint16), value_(x) {
}
Variant::Variant(int32_t x) noexcept : type_tag_(TypeTag::int32), value_(x) {
}
Variant::Variant(uint32_t x) noexcept : type_tag_(TypeTag::uint32), value_(x) {
}
Variant::Variant(int64_t x) noexcept : type_tag_(TypeTag::int64), value_(x) {
}
Variant::Variant(uint64_t x) noexcept : type_tag_(TypeTag::uint64), value_(x) {
}
Variant::Variant(double x) noexcept : type_tag_(TypeTag::double_), value_(x) {
}

Variant::Variant(char const* const& x)
    : type_tag_(TypeTag::string), value_(new std::string(x)) {
}

Variant::Variant(std::string const& x)
    : type_tag_(TypeTag::string), value_(new std::string(x)) {
}
Variant::Variant(std::string&& x)
    : type_tag_(TypeTag::string), value_(new std::string(std::move(x))) {
}

Variant::Variant(Vec const& x) : type_tag_(TypeTag::vec), value_(new Vec(x)) {
}
Variant::Variant(Vec&& x) : type_tag_(TypeTag::vec), value_(new Vec(std::move(x))) {
}

Variant::Variant(Map const& x) : type_tag_(TypeTag::map), value_(new Map(x)) {
}
Variant::Variant(Map&& x) : type_tag_(TypeTag::map), value_(new Map(std::move(x))) {
}

struct Variant::Impl {
    static inline ValueType copy(TypeTag tag, ValueType x) {
        ValueType ret;
        switch (tag) {
        case TypeTag::string:
            ret = new std::string(*reinterpret_cast<std::string*>(x.ptr));
            break;
        case TypeTag::vec: ret = new Vec(*reinterpret_cast<Vec*>(x.ptr)); break;
        case TypeTag::map: ret = new Map(*reinterpret_cast<Map*>(x.ptr)); break;
        default: ret = x; break;
        }
        return ret;
    }

    struct ToJson;
};

Variant::Variant(Variant const& rhs)
    : type_tag_(rhs.type_tag_), value_(Impl::copy(type_tag_, rhs.value_)) {
}

Variant& Variant::operator=(Variant const& rhs) {
    auto const value = Impl::copy(rhs.type_tag_, rhs.value_);
    this->~Variant();
    type_tag_ = rhs.type_tag_;
    value_ = value;
    return *this;
}

Variant::Variant(Variant&& rhs) noexcept : type_tag_(rhs.type_tag_), value_(rhs.value_) {
    rhs.type_tag_ = TypeTag::null;
    rhs.value_.null_ = {};
}

Variant& Variant::operator=(Variant&& rhs) noexcept {
    Variant tmp(std::move(rhs));
    this->~Variant();
    type_tag_ = tmp.type_tag_;
    value_ = tmp.value_;
    tmp.type_tag_ = TypeTag::null;
    return *this;
}

namespace {

template <typename T, typename = void>
struct GetHelper final : GetHelper<T, When<true>> {};

template <typename T, bool condition>
struct GetHelper<T, When<condition>> {
    static_assert(!std::is_same_v<Variant::NullType, std::decay_t<T>>);

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

template <typename A, typename B>
constexpr auto same_sign_v = std::is_signed_v<A> == std::is_signed_v<B>;

// Casts `U` to `T` if a specific value of `U` is representable in `T` else throws
// `VariantIntegralOverflow`
template <typename T, typename U, typename = void>
struct IntegralCheckedCast : IntegralCheckedCast<T, U, When<true>> {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>);
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<T, U, When<same_sign_v<T, U> && sizeof(T) >= sizeof(U)>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename T, typename U>
struct IntegralCheckedCast<
        T, U,
        When<same_sign_v<T, U> && sizeof(T) < sizeof(U) && !std::is_same_v<T, bool>>> {
    static T apply(U x) {
        if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max()) {
            throw VariantIntegralOverflow(std::string(typeName(boost::hana::type_c<T>)),
                                          std::to_string(x));
        } else {
            return static_cast<T>(x);
        }
    }
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<
        T, U,
        When<std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) > sizeof(U))>> {
    static T apply(U x) noexcept {
        return x;
    }
};

template <typename T, typename U>
struct IntegralCheckedCast<
        T, U,
        When<std::is_signed_v<T> && std::is_unsigned_v<U> && (sizeof(T) <= sizeof(U))>> {
    static T apply(U x) {
#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation
        if (x > std::numeric_limits<T>::max()) {
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif
            throw VariantIntegralOverflow(std::string(typeName(boost::hana::type_c<T>)),
                                          std::to_string(x));
        }
        return static_cast<T>(x);
    }
}; // namespace

template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<std::is_unsigned_v<T> && std::is_signed_v<U> &&
                                sizeof(T) >= sizeof(U) && !std::is_same_v<T, bool>>> {
    static T apply(U x) {
        if (x < 0) {
            throw VariantIntegralOverflow(std::string(typeName(boost::hana::type_c<T>)),
                                          std::to_string(x));
        } else {
            return static_cast<T>(x);
        }
    }
};

template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<std::is_unsigned_v<T> && std::is_signed_v<U> &&
                                sizeof(T) < sizeof(U) && !std::is_same_v<T, bool>>> {
    static T apply(U x) {
#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation
        if (x < 0 || x > std::numeric_limits<T>::max()) {
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif
            throw VariantIntegralOverflow(std::string(typeName(boost::hana::type_c<T>)),
                                          std::to_string(x));
        }
        return static_cast<T>(x);
    }
}; // namespace serialize

template <typename U>
struct IntegralCheckedCast<bool, U,
                           When<!std::is_same_v<bool, U> && std::is_unsigned_v<U>>> {
    static bool apply(U x) {
        if (x > 1) {
            throw VariantIntegralOverflow(
                    std::string(typeName(boost::hana::type_c<bool>)), std::to_string(x));
        } else {
            return x;
        }
    }
};

template <typename U>
struct IntegralCheckedCast<bool, U,
                           When<!std::is_same_v<bool, U> && std::is_signed_v<U>>> {
    static bool apply(U x) {
        if (x > 1 || x < 0) {
            throw VariantIntegralOverflow(
                    std::string(typeName(boost::hana::type_c<bool>)), std::to_string(x));
        } else {
            return x;
        }
    }
};

template <typename T>
struct IntegralCheckedCast<T, bool> {
    static T apply(bool x) noexcept {
        return x;
    }
};

template <typename Dst>
struct IntegralCheckedCastT {
    template <typename Src>
    Dst operator()(Src x) const
            noexcept(noexcept(IntegralCheckedCast<Dst, Src>::apply(x))) {
        return IntegralCheckedCast<Dst, Src>::apply(x);
    }
};

template <typename T>
constexpr IntegralCheckedCastT<T> integralCheckedCast;

template <typename T>
struct GetHelper<T, When<std::is_integral_v<T>>> {
    [[noreturn]] static T apply(Variant::NullType) {
        throw VariantEmpty(boost::hana::type_c<T>);
    }
    static T apply(bool x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(char x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(int16_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(uint16_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(int32_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(uint32_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(int64_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    static T apply(uint64_t x) noexcept(noexcept(integralCheckedCast<T>(x))) {
        return integralCheckedCast<T>(x);
    }
    [[noreturn]] static T apply(double) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<double>);
    }
    [[noreturn]] static T apply(std::string) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<std::string>);
    }
    [[noreturn]] static T apply(Variant::Vec) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<Variant::Vec>);
    }
    [[noreturn]] static T apply(Variant::Map) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<Variant::Map>);
    }
};

template <typename T>
struct GetHelper<T, When<std::is_floating_point_v<T>>> {
    [[noreturn]] static T apply(Variant::NullType) {
        throw VariantEmpty(boost::hana::type_c<T>);
    }
    static T apply(bool x) noexcept {
        return x;
    }
    static T apply(char x) noexcept {
        return x;
    }
    static T apply(int16_t x) noexcept {
        return x;
    }
    static T apply(uint16_t x) noexcept {
        return x;
    }
    static T apply(int32_t x) noexcept {
        return x;
    }
    static T apply(uint32_t x) noexcept {
        return x;
    }
    static T apply(int64_t x) noexcept {
        return x;
    }
    static T apply(uint64_t x) noexcept {
        return x;
    }
    static T apply(double x) noexcept {
        return x;
    }
    static T apply(std::string) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<std::string>);
    }
    static T apply(Variant::Vec) {
        throw VariantBadType(boost::hana::type_c<T>, boost::hana::type_c<Variant::Vec>);
    }
    static T apply(Variant::Map) {
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
    case TypeTag::null: return GetHelper<T>::apply(value_.null_);
    case TypeTag::boolean: return GetHelper<T>::apply(value_.bool_);
    case TypeTag::char_: return GetHelper<T>::apply(value_.char_);
    case TypeTag::uchar: return GetHelper<T>::apply(value_.uchar);
    case TypeTag::int16: return GetHelper<T>::apply(value_.int16);
    case TypeTag::uint16: return GetHelper<T>::apply(value_.uint16);
    case TypeTag::int32: return GetHelper<T>::apply(value_.int32);
    case TypeTag::uint32: return GetHelper<T>::apply(value_.uint32);
    case TypeTag::int64: return GetHelper<T>::apply(value_.int64);
    case TypeTag::uint64: return GetHelper<T>::apply(value_.uint64);
    case TypeTag::double_: return GetHelper<T>::apply(value_.double_);
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
    if (tag == TypeTag::null) {                                                          \
        return x;                                                                        \
    } else {                                                                             \
        return getHelper<T>(tag, value);                                                 \
    }                                                                                    \
    (void)x

char Variant::char8() const {
    return getHelper<char>(type_tag_, value_);
}

char Variant::char8Or(char x) const {
    GET_HELPER(char, type_tag_, value_, x);
}

unsigned char Variant::uchar8() const {
    return getHelper<unsigned char>(type_tag_, value_);
}

unsigned char Variant::uchar8Or(unsigned char x) const {
    GET_HELPER(unsigned char, type_tag_, value_, x);
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

bool Variant::operator==(Variant const& rhs) const noexcept {
    if (type_tag_ != rhs.type_tag_) {
        return false;
    } else {
        switch (type_tag_) {
        case TypeTag::null: return true;
        case TypeTag::boolean: return value_.bool_ == rhs.value_.bool_;
        case TypeTag::char_: return value_.char_ == rhs.value_.char_;
        case TypeTag::uchar: return value_.uchar == rhs.value_.uchar;
        case TypeTag::int16: return value_.int16 == rhs.value_.int16;
        case TypeTag::uint16: return value_.uint16 == rhs.value_.uint16;
        case TypeTag::int32: return value_.int32 == rhs.value_.int32;
        case TypeTag::uint32: return value_.uint32 == rhs.value_.uint32;
        case TypeTag::int64: return value_.int64 == rhs.value_.int64;
        case TypeTag::uint64: return value_.uint64 == rhs.value_.uint64;
        case TypeTag::double_: return value_.double_ == rhs.value_.double_;
        case TypeTag::string:
            return *reinterpret_cast<std::string*>(value_.ptr) ==
                   *reinterpret_cast<std::string*>(rhs.value_.ptr);
        case TypeTag::vec:
            return *reinterpret_cast<Variant::Vec*>(value_.ptr) ==
                   *reinterpret_cast<Variant::Vec*>(rhs.value_.ptr);
        case TypeTag::map:
            return *reinterpret_cast<Variant::Map*>(value_.ptr) ==
                   *reinterpret_cast<Variant::Map*>(rhs.value_.ptr);
        }
        return false;
    }
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
            ptrs.back()->modifyMap()[std::move(key)] = Variant(std::move(x));
            break;
        case Variant::TypeTag::vec:
            ptrs.back()->modifyVec().push_back(Variant(std::move(x)));
            break;
        default: *ptrs.back() = Variant(std::move(x));
        }
    }

    template <class T>
    Variant* val2(T&& x) {
        switch (ptrs.back()->type()) {
        case Variant::TypeTag::map:
            return &(ptrs.back()->modifyMap()[std::move(key)] = Variant(std::move(x)));
        case Variant::TypeTag::vec:
            ptrs.back()->modifyVec().push_back(Variant(std::move(x)));
            return &ptrs.back()->modifyVec().back();
        default: return &(*ptrs.back() = Variant(std::move(x)));
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

    explicit ToJson(Variant const& var) : var(var) {
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
        case TypeTag::null: dst.Null(); break;
        case TypeTag::boolean: dst.Bool(var.value_.bool_); break;
        case TypeTag::char_: dst.Int(var.value_.char_); break;
        case TypeTag::uchar: dst.Uint(var.value_.uchar); break;
        case TypeTag::int16: dst.Int(var.value_.int16); break;
        case TypeTag::uint16: dst.Uint(var.value_.uint16); break;
        case TypeTag::int32: dst.Int(var.value_.int32); break;
        case TypeTag::uint32: dst.Uint(var.value_.uint32); break;
        case TypeTag::int64: dst.Int64(var.value_.int64); break;
        case TypeTag::uint64: dst.Uint64(var.value_.uint64); break;
        case TypeTag::double_: dst.Double(var.value_.double_); break;
        case TypeTag::string: {
            auto const str = reinterpret_cast<std::string*>(var.value_.ptr);
            dst.String(str->c_str(), static_cast<unsigned int>(str->size()), true);
            break;
        }
        case TypeTag::vec: {
            dst.StartArray();
            auto const vec = reinterpret_cast<Variant::Vec*>(var.value_.ptr);
            for (auto const& var : *vec) { apply(dst, var); }
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

std::ostream& operator<<(std::ostream& os, Variant const& var) {
    using TypeTag = Variant::TypeTag;
    switch (var.type_tag_) {
    case TypeTag::null: os << "Null"; break;
    case TypeTag::boolean: os << var.value_.bool_; break;
    case TypeTag::char_: os << var.value_.char_; break;
    case TypeTag::uchar: os << var.value_.uchar; break;
    case TypeTag::int16: os << var.value_.int16; break;
    case TypeTag::uint16: os << var.value_.uint16; break;
    case TypeTag::int32: os << var.value_.int32; break;
    case TypeTag::uint32: os << var.value_.uint32; break;
    case TypeTag::int64: os << var.value_.int64; break;
    case TypeTag::uint64: os << var.value_.uint64; break;
    case TypeTag::double_: os << var.value_.double_; break;
    case TypeTag::string: os << *reinterpret_cast<std::string*>(var.value_.ptr); break;
    case TypeTag::vec: {
        auto const vec = reinterpret_cast<Variant::Vec*>(var.value_.ptr);
        auto const l = vec->size() - 1;
        std::size_t i = 0;
        os << "[ ";
        for (auto const& x : *vec) { os << x << ((i++ == l) ? " " : ", "); }
        os << "]";
        break;
    }
    case TypeTag::map: {
        auto const map = reinterpret_cast<Variant::Map*>(var.value_.ptr);
        os << "{ ";
        for (auto const& [key, x] : *map) { os << key << ": " << x << "; "; }
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
    case TypeTag::null: return typeid(NullType);
    case TypeTag::boolean: return typeid(bool);
    case TypeTag::char_: return typeid(char);
    case TypeTag::uchar: return typeid(unsigned char);
    case TypeTag::int16: return typeid(int16_t);
    case TypeTag::uint16: return typeid(uint16_t);
    case TypeTag::int32: return typeid(int32_t);
    case TypeTag::uint32: return typeid(uint32_t);
    case TypeTag::int64: return typeid(int64_t);
    case TypeTag::uint64: return typeid(uint64_t);
    case TypeTag::double_: return typeid(double);
    case TypeTag::string: return typeid(std::string);
    case TypeTag::vec: return typeid(Vec);
    case TypeTag::map: return typeid(Map);
    }
}
#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

bool Variant::null() const noexcept {
    return type_tag_ == TypeTag::null;
}

} // namespace serialize
