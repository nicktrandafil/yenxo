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

#include <serialize/meta.hpp>
#include <serialize/type_name.hpp>
#include <serialize/variant.hpp>

#include <serialize/pimpl_impl.hpp>

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <deque>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

namespace serialize {

using Val = Variant::Types::push_front<std::monostate>::rebind<std::variant>;

struct Variant::Impl {
    Val m;
};

Variant::Variant() = default;

Variant::~Variant() = default;

Variant::Variant(bool x) : impl(x) {}
Variant::Variant(char x) : impl(x) {}
Variant::Variant(unsigned char x) : impl(x) {}
Variant::Variant(short int x) : impl(x) {}
Variant::Variant(unsigned short int x) : impl(x) {}
Variant::Variant(int x) : impl(x) {}
Variant::Variant(unsigned int x) : impl(x) {}
Variant::Variant(signed long x) : impl(x) {}
Variant::Variant(unsigned long x) : impl(x) {}
Variant::Variant(double x) : impl(x) {}

Variant::Variant(char const* const& x) : Variant(std::string(x)) {}
Variant::Variant(std::string const& x) : impl(x) {}
Variant::Variant(std::string&& x) : impl(std::move(x)) {}

Variant::Variant(Vec const& x) : impl(x) {}
Variant::Variant(Vec&& x) : impl(std::move(x)) {}

Variant::Variant(Map const& x) : impl(x) {}
Variant::Variant(Map&& x) : impl(std::move(x)) {}

Variant::Variant(Variant const&) = default;
Variant& Variant::operator=(Variant const&) = default;

Variant::Variant(Variant&&) noexcept = default;
Variant& Variant::operator=(Variant&&) noexcept = default;

namespace {

template <typename T, typename = void>
struct GetHelper : GetHelper<T, When<true>> {};

template <typename T, bool condition>
struct GetHelper<T, When<condition>> {
    T const& operator()(T const& x) const noexcept { return x; }
    [[noreturn]] T const& operator()(std::monostate) const {
        throw VariantEmpty(type_c<T>);
    }
    template <typename U>
    [[noreturn]] T const& operator()(U const&) const {
        throw VariantBadType(type_c<T>, type_c<U>);
    }
};

template <typename A, typename B>
constexpr auto same_sign_v = !(std::is_signed_v<A> ^ std::is_signed_v<B>);

///
/// Check if a specific value of type `U` is representable in type `T`
///
template <typename T, typename U, typename = void>
struct IntegralCheckedCast : IntegralCheckedCast<T, U, When<true>> {
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>);
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<
                               same_sign_v<U, T> &&
                               (sizeof(T) >= sizeof(U))>> {
    static T apply(U x) noexcept { return x; }
};

// safe conversion
template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<
                               std::is_signed_v<T> &&
                               std::is_unsigned_v<U> &&
                               (sizeof(T) > sizeof(U))>> {
    static T apply(U x) noexcept { return x; }
};

template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<
                               std::is_signed_v<T> &&
                               std::is_unsigned_v<U> &&
                               (sizeof(T) <= sizeof(U))>> {
    static T apply(U x) {

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

        if (x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                std::string(unqualifiedTypeName<T>()),
                std::to_string(x));
        }

        return T(x);
    }
}; // namespace

template <typename T, typename U>
struct IntegralCheckedCast<T, U,
                           When<
                               std::is_unsigned_v<T> &&
                               std::is_signed_v<U>>> {
    static T apply(U x) {

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

        if (x < 0 || x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                std::string(unqualifiedTypeName<T>()),
                std::to_string(x));
        }

        return x;
    }
}; // namespace serialize

template <typename T, typename U, bool condition>
struct IntegralCheckedCast<T, U, When<condition>> {
    static T apply(U x) {

#if defined(__GNUG__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // safe comparation

        if (x < std::numeric_limits<T>::min() ||
            x > std::numeric_limits<T>::max()) {

#pragma GCC diagnostic pop
#else
#error The compiler not supported
#endif

            throw VariantIntegralOverflow(
                std::string(unqualifiedTypeName<T>()),
                std::to_string(x));
        }
        return T(x);
    }
};

template <typename T>
struct IntegralCheckedCast<T, bool> {
    static T apply(bool x) { return x; }
};

template <typename Dst>
struct IntegralCheckedCastT {
    template <typename Src>
    Dst operator()(Src x) const {
        return IntegralCheckedCast<Dst, Src>::apply(x);
    }
};

template <typename T>
constexpr IntegralCheckedCastT<T> integralCheckedCast;

template <typename T>
struct GetHelper<T, When<std::is_integral_v<T>>> {
    [[noreturn]] T operator()(std::monostate) const {
        throw VariantEmpty(type_c<T>);
    }

    T operator()(bool x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(char x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(short int x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(unsigned short int x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(int x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(unsigned int x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(signed long x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(unsigned long x) const {
        return integralCheckedCast<T>(x);
    }

    T operator()(double) const { throw VariantBadType(type_c<T>, type_c<double>); }
    T operator()(std::string) const { throw VariantBadType(type_c<T>, type_c<std::string>); }
    T operator()(Variant::Vec) const { throw VariantBadType(type_c<T>, type_c<Variant::Vec>); }
    T operator()(Variant::Map) const { throw VariantBadType(type_c<T>, type_c<Variant::Map>); }
};

template <typename T>
struct GetHelper<T, When<std::is_reference_v<T>>> {
    T operator()(T x) const noexcept { return x; }
    [[noreturn]] T operator()(std::monostate) const {
        throw VariantEmpty(type_c<std::remove_reference_t<T>>);
    }
    template <typename U>
    [[noreturn]] T operator()(U const&) const {
        throw VariantBadType(type_c<std::remove_reference_t<T>>, type_c<U>);
    }
};

template <typename T>
struct GetOrHelper : GetHelper<T> {
    GetOrHelper(T const& t) : t(t) {}

    using GetHelper<T>::operator();
    decltype(GetHelper<T>()(std::monostate())) operator()(std::monostate) const {
        return t;
    }

    T const& t;
};

} // namespace

char Variant::character() const {
    return std::visit(GetHelper<char>(), impl->m);
}

char Variant::characterOr(char x) const {
    return std::visit(GetOrHelper<char>{x}, impl->m);
}

unsigned char Variant::uchar() const {
    return std::visit(GetHelper<unsigned char>(), impl->m);
}

unsigned char Variant::ucharOr(unsigned char x) const {
    return std::visit(GetOrHelper<unsigned char>{x}, impl->m);
}

short int Variant::shortInt() const {
    return std::visit(GetHelper<short int>(), impl->m);
}

short int Variant::shortIntOr(short int x) const {
    return std::visit(GetOrHelper<short int>{x}, impl->m);
}

unsigned short int Variant::ushortInt() const {
    return std::visit(GetHelper<unsigned short int>(), impl->m);
}

unsigned short int Variant::ushortIntOr(unsigned short int x) const {
    return std::visit(GetOrHelper<unsigned short int>{x}, impl->m);
}

bool Variant::boolean() const {
    return std::visit(GetHelper<bool>(), impl->m);
}

bool Variant::booleanOr(bool x) const {
    return std::visit(GetOrHelper<bool>{x}, impl->m);
}

int Variant::integer() const {
    return std::visit(GetHelper<int>(), impl->m);
}

int Variant::integerOr(int x) const {
    return std::visit(GetOrHelper<int>{x}, impl->m);
}

unsigned int Variant::uint() const {
    return std::visit(GetHelper<unsigned int>(), impl->m);
}

unsigned int Variant::uintOr(unsigned int x) const {
    return std::visit(GetOrHelper<unsigned int>{x}, impl->m);
}

signed long Variant::longInt() const {
    return std::visit(GetHelper<signed long>(), impl->m);
}

signed long Variant::longInteOr(signed long x) const {
    return std::visit(GetOrHelper<signed long>{x}, impl->m);
}

unsigned long Variant::ulongInt() const {
    return std::visit(GetHelper<unsigned long>(), impl->m);
}

unsigned long Variant::ulongIntOr(unsigned long x) const {
    return std::visit(GetOrHelper<unsigned long>{x}, impl->m);
}

double Variant::floating() const {
    return std::visit(GetHelper<double>(), impl->m);
}

double Variant::floatingOr(double x) const {
    return std::visit(GetOrHelper<double>{x}, impl->m);
}

std::string const& Variant::str() const {
    return std::visit(GetHelper<std::string>(), impl->m);
}

std::string Variant::strOr(std::string const& x) const {
    return std::visit(GetOrHelper<std::string>{x}, impl->m);
}

Variant::Vec const& Variant::vec() const {
    return std::visit(GetHelper<Vec>(), impl->m);
}

Variant::Vec Variant::vecOr(Vec const& x) const {
    return std::visit(GetOrHelper<Vec>{x}, impl->m);
}

Variant::Vec& Variant::modifyVec() {
    return std::visit(GetHelper<Vec&>(), impl->m);
}

Variant::Map const& Variant::map() const {
    return std::visit(GetHelper<Map>(), impl->m);
}

Variant::Map Variant::mapOr(Map const& x) const {
    return std::visit(GetOrHelper<Map>{x}, impl->m);
}

Variant::Map& Variant::modifyMap() {
    return std::visit(GetHelper<Map&>(), impl->m);
}

bool Variant::operator==(Variant const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}

bool Variant::operator!=(Variant const& rhs) const noexcept {
    return impl->m != rhs.impl->m;
}

namespace {

using namespace rapidjson;

/// RapidJSON visitor
template <typename Ch>
struct FromRapidJsonValue {
    bool Null() {
        val(Variant());
        return true;
    }
    bool Bool(bool b) {
        val(Variant(b));
        return true;
    }
    bool Int(int i) {
        val(Variant(i));
        return true;
    }
    bool Uint(unsigned u) {
        val(Variant(u));
        return true;
    }
    bool Int64(int64_t i64) {
        val(Variant(i64));
        return true;
    }
    bool Uint64(uint64_t u64) {
        val(Variant(u64));
        return true;
    }
    bool Double(double d) {
        val(Variant(d));
        return true;
    }

    bool String(const Ch* str, SizeType length, bool) {
        val(Variant(std::string(str, length)));
        return true;
    }

    bool StartObject() {
        stack.push_back(KeyCarriedMap());
        return true;
    }

    bool Key(const Ch* str, SizeType length, bool) {
        key(std::string(str, length));
        return true;
    }

    bool EndObject(SizeType) {
        std::visit([this](auto& x) { finish_v(x,
                                              std::visit(res_v, std::move(stack.back()))); },
                   *std::prev(stack.end(), 2));
        stack.pop_back();
        return true;
    }

    bool StartArray() {
        stack.push_back(Variant::Vec());
        return true;
    }

    bool EndArray(SizeType) {
        std::visit([this](auto& x) { finish_v(x,
                                              std::visit(res_v, std::move(stack.back()))); },
                   *std::prev(stack.end(), 2));
        stack.pop_back();
        return true;
    }

    struct KeyCarriedMap {
        std::string key;
        Variant::Map map;
    };

    using Stack = std::variant<Variant, KeyCarriedMap, Variant::Vec>;

    struct Val {
        Variant& operator()(Variant& x) const { return x; }
        Variant& operator()(KeyCarriedMap& x) const { return x.map[x.key]; }
        Variant& operator()(Variant::Vec& x) const {
            x.push_back(Variant());
            return x.back();
        }
    };

    struct Key {
        [[noreturn]] std::string& operator()(Variant&) const {
            throw std::runtime_error("JSON structure error");
        }

        [[noreturn]] std::string& operator()(Variant::Vec&) const {
            throw std::runtime_error("JSON structure error");
        }

        std::string& operator()(KeyCarriedMap& x) const {
            return x.key;
        }
    };

    struct Finish {
        void operator()(Variant& x, Variant&& y) const {
            x = std::move(y);
        }

        void operator()(Variant::Vec& x, Variant&& y) const {
            x.push_back(std::move(y));
        }

        void operator()(KeyCarriedMap& x, Variant&& y) const {
            x.map[x.key] = std::move(y);
        }
    };

    struct Res {
        Variant operator()(Variant&& x) const {
            return std::move(x);
        }

        Variant operator()(Variant::Vec&& x) const {
            return Variant(std::move(x));
        }

        Variant operator()(KeyCarriedMap&& x) const {
            return Variant(std::move(x).map);
        }
    };

    void val(Variant&& x) {
        std::visit(value_v, stack.back()) = std::move(x);
    }

    void key(std::string&& x) {
        std::visit(key_v, stack.back()) = std::move(x);
    }

    std::vector<Stack> stack{Variant()};
    Val const value_v{};
    struct Key const key_v{};
    Finish const finish_v{};
    Res const res_v{};
};

} // namespace

Variant Variant::from(Value const& json) {
    FromRapidJsonValue<Value::Ch> ser;
    json.Accept(ser);
    return std::visit(ser.res_v, std::move(ser.stack.front()));
}

Variant Variant::fromJson(std::string const& json) {
    rapidjson::Document d;
    if (d.Parse(json.c_str()).HasParseError()) {
        throw std::runtime_error(rapidjson::GetParseError_En(d.GetParseError()));
    }
    return Variant::from(d);
}

rapidjson::Document& Variant::to(rapidjson::Document& json) const {
    std::visit(Overload{
                   [&](std::monostate) { json.SetNull(); },
                   [&](bool x) { json.SetBool(x); },
                   [&](char x) { json.SetInt(x); },
                   [&](short int x) { json.SetInt(x); },
                   [&](unsigned short int x) { json.SetUint(x); },
                   [&](int x) { json.SetInt(x); },
                   [&](unsigned int x) { json.SetUint(x); },
                   [&](signed long x) { json.SetInt64(x); },
                   [&](unsigned long x) { json.SetUint64(x); },
                   [&](double x) { json.SetDouble(x); },
                   [&](std::string const& x) {
                       json.SetString(x.c_str(), json.GetAllocator());
                   },
                   [&](Variant::Vec const& vec) {
                       json.SetArray();
                       for (auto const& x : vec) {
                           rapidjson::Document tmp;
                           x.to(tmp);
                           json.PushBack(
                               rapidjson::Value(tmp.Move(), json.GetAllocator()),
                               json.GetAllocator());
                       }
                   },
                   [&](Variant::Map const& map) {
                       json.SetObject();
                       for (auto const& [key, x] : map) {
                           rapidjson::Document tmp;
                           x.to(tmp);
                           json.AddMember(
                               rapidjson::Value(key.c_str(), json.GetAllocator()),
                               rapidjson::Value(tmp.Move(), json.GetAllocator()),
                               json.GetAllocator());
                       }
                   }},
               impl->m);

    return json;
}

std::string Variant::toJson() const {
    rapidjson::Document doc;
    to(doc);

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);

    return sb.GetString();
}

std::ostream& operator<<(std::ostream& os, Variant const& var) {
    std::visit(Overload{
                   [&](auto integral) { os << std::to_string(integral); },
                   [&](std::monostate) { os << "Null"; },
                   [&](std::string const& str) { os << str; },
                   [&](Variant::Map const& map) {
                       os << "{ ";
                       for (auto const& [key, x] : map) {
                           os << key << ": " << x << "; ";
                       }
                       os << "}";
                   },
                   [&](Variant::Vec const& vec) {
                       auto const l = vec.size() - 1;
                       std::size_t i = 0;
                       os << "[ ";
                       for (auto const& x : vec) {
                           os << x << ((i++ == l) ? " " : ", ");
                       }
                       os << "]";
                   }},
               var.impl->m);
    return os;
}

std::type_info const& Variant::typeInfo() const {
    return std::visit(Overload{
                          [&](auto val) -> std::type_info const& { return typeid(val); }},
                      impl->m);
}

bool Variant::empty() const noexcept {
    return std::visit(Overload{
                          [](std::monostate) { return true; },
                          [](auto) { return false; }},
                      impl->m);
}

} // namespace serialize
