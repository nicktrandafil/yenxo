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

#include <serialize/enum_traits.hpp>
#include <serialize/meta.hpp>

#include <rapidjson/fwd.h>

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace serialize {

/// \ingroup group-datatypes
/// Serialized object representation. Think of it as a DOM object.
class Variant {
public:
    struct NullType {};
    using Map = std::unordered_map<std::string, Variant>;
    using Vec = std::vector<Variant>;

    enum class TypeTag : uint8_t {
        null,
        boolean,
        char_,
        uchar,
        int16,
        uint16,
        int32,
        uint32,
        int64,
        uint64,
        double_,
        string,
        vec,
        map
    };

    ~Variant() noexcept;

    Variant() noexcept;

    Variant(NullType x) noexcept;
    explicit Variant(bool) noexcept;
    explicit Variant(char) noexcept;
    explicit Variant(unsigned char) noexcept;
    explicit Variant(int16_t) noexcept;
    explicit Variant(uint16_t) noexcept;
    explicit Variant(int32_t) noexcept;
    explicit Variant(uint32_t) noexcept;
    explicit Variant(int64_t) noexcept;
    explicit Variant(uint64_t) noexcept;
    explicit Variant(double) noexcept;

    explicit Variant(char const* const&);
    explicit Variant(std::string const&);
    explicit Variant(std::string&&);

    explicit Variant(Vec const&);
    explicit Variant(Vec&&);

    explicit Variant(Map const&);
    explicit Variant(Map&&);

    template <typename T, typename = decltype(T::toVariant(std::declval<T>()))>
    explicit Variant(T const& x) : Variant(T::toVariant(x)) {
    }

    template <typename T, typename = decltype(T::fromVariant(std::declval<Variant>()))>
    explicit operator T() const {
        return T::fromVariant(*this);
    }

    // copy
    Variant(Variant const& rhs);
    Variant& operator=(Variant const& rhs);

    // move
    Variant(Variant&& rhs) noexcept;
    Variant& operator=(Variant&& rhs) noexcept;

    /// Get as `T` or `x` if the object is null
    template <typename T>
    T asOr(T x) const;

    /// Get bool
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    bool boolean() const;
    explicit operator bool() const {
        return boolean();
    }

    /// Get bool or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    bool booleanOr(bool x) const;

    /// Get char
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    char char8() const;
    explicit operator char() const {
        return char8();
    }

    /// Get char or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    char char8Or(char x) const;

    /// Get unsigned char
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    unsigned char uchar8() const;
    explicit operator unsigned char() const {
        return uchar8();
    }

    /// Get unsigned char or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    unsigned char uchar8Or(unsigned char x) const;

    /// Get int16
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    int16_t int16() const;
    explicit operator int16_t() const {
        return int16();
    }

    /// Get int16 or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    int16_t int16Or(int16_t x) const;

    /// Get uint16
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    uint16_t uint16() const;
    explicit operator uint16_t() const {
        return uint16();
    }

    /// Get uint16 or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    uint16_t uint16Or(uint16_t) const;

    /// Get int32
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    int32_t int32() const;
    explicit operator int32_t() const {
        return int32();
    }

    /// Get int32 or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    int32_t int32Or(int32_t x) const;

    /// Get uint32
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    uint32_t uint32() const;
    explicit operator uint32_t() const {
        return uint32();
    }

    /// Get uint32 or `x` if the object is null
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    uint32_t uint32Or(uint32_t x) const;

    /// Get int64 or `x` if the object is null
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    int64_t int64() const;
    explicit operator int64_t() const {
        return int64();
    }

    /// Get int64 or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    int64_t int64Or(int64_t x) const;

    /// Get uint64 or `x` if the object is null
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    uint64_t uint64() const;
    explicit operator uint64_t() const {
        return uint64();
    }

    /// Get uint64 or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    uint64_t uint64Or(uint64_t x) const;

    /// Get double
    /// \throw VariantEmpty, VariantBadType, VariantIntegralOverflow
    double floating() const;
    explicit operator double() const {
        return floating();
    }

    /// Get int or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    double floatingOr(double x) const;

    /// Get string
    /// \throw VariantEmpty, VariantBadType
    std::string const& str() const;
    explicit operator std::string const&() const {
        return str();
    }

    /// Get string or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    std::string strOr(std::string const& x) const;

    /// Get Vec
    /// \throw VariantEmpty, VariantBadType
    Vec const& vec() const;
    explicit operator Vec const&() const {
        return vec();
    }
    Vec& modifyVec();

    /// Get Vec or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    Vec vecOr(Vec const& x) const;

    /// Get Map
    /// \throw VariantEmpty, VariantBadType
    Map const& map() const;
    explicit operator Map const&() const {
        return map();
    }
    Map& modifyMap();

    /// Get Map or `x` if the object is null
    /// \throw VariantBadType, VariantIntegralOverflow
    Map mapOr(Map const& x) const;

    /// Check if Variant contains null
    bool null() const noexcept;

    bool operator==(Variant const& rhs) const noexcept;
    bool operator!=(Variant const& rhs) const noexcept;

    /// \ingroup group-json
    /// @{
    static Variant from(rapidjson::Value const& json);

    /// \throw std::runtime_error on `json` parse
    static Variant fromJson(std::string const& json);

    rapidjson::Document& to(rapidjson::Document& json) const;

    std::string toJson() const;
    /// @}

    friend std::ostream& operator<<(std::ostream& os, Variant const& var);

    std::type_info const& typeInfo() const noexcept;

    TypeTag type() const noexcept {
        return type_tag_;
    }

    static constexpr std::string_view typeName() noexcept {
        return "variant";
    }

    /// Test if the value is a scalar
    inline bool isScalar() const noexcept {
        return type_tag_ != TypeTag::map && type_tag_ != TypeTag::vec;
    }

    // list of supported types
    using Types =
            S<NullType, bool, char, unsigned char, int64_t, uint64_t, int32_t, uint32_t,
              int64_t, uint64_t, double, std::string, Variant::Vec, Variant::Map>;

private:
    struct Impl;
    TypeTag type_tag_;
    union ValueType {
        ValueType() = default;
        ValueType(NullType x) noexcept : null_(x) {
        }
        ValueType(bool x) noexcept : bool_(x) {
        }
        ValueType(char x) noexcept : char_(x) {
        }
        ValueType(unsigned char x) noexcept : uchar(x) {
        }
        ValueType(int16_t x) noexcept : int16(x) {
        }
        ValueType(uint16_t x) noexcept : uint16(x) {
        }
        ValueType(int32_t x) noexcept : int32(x) {
        }
        ValueType(uint32_t x) noexcept : uint32(x) {
        }
        ValueType(int64_t x) noexcept : int64(x) {
        }
        ValueType(uint64_t x) noexcept : uint64(x) {
        }
        ValueType(double x) noexcept : double_(x) {
        }
        ValueType(void* x) noexcept : ptr(x) {
        }
        NullType null_;
        bool bool_;
        char char_;
        unsigned char uchar;
        int16_t int16;
        uint16_t uint16;
        int32_t int32;
        uint32_t uint32;
        int64_t int64;
        uint64_t uint64;
        double double_;
        void* ptr;
    } value_;
};

using VariantMap = std::unordered_map<std::string, Variant>;
using VariantVec = std::vector<Variant>;

template <>
inline bool Variant::asOr<bool>(bool x) const {
    return booleanOr(x);
}
template <>
inline char Variant::asOr<char>(char x) const {
    return char8Or(x);
}
template <>
inline unsigned char Variant::asOr<unsigned char>(unsigned char x) const {
    return uchar8Or(x);
}
template <>
inline int16_t Variant::asOr<int16_t>(int16_t x) const {
    return int16Or(x);
}
template <>
inline uint16_t Variant::asOr<uint16_t>(uint16_t x) const {
    return uint16Or(x);
}
template <>
inline int32_t Variant::asOr<int32_t>(int32_t x) const {
    return int32Or(x);
}
template <>
inline uint32_t Variant::asOr<uint32_t>(uint32_t x) const {
    return uint32Or(x);
}
template <>
inline int64_t Variant::asOr<int64_t>(int64_t x) const {
    return int64Or(x);
}
template <>
inline uint64_t Variant::asOr<uint64_t>(uint64_t x) const {
    return uint64Or(x);
}

template <>
struct EnumTraits<Variant::TypeTag> {
    using Enum = Variant::TypeTag;
    static constexpr size_t const count = 14;
    static constexpr std::array<Enum, count> const values = {
            Enum::null,    Enum::boolean, Enum::char_,  Enum::uchar, Enum::int16,
            Enum::uint16,  Enum::int32,   Enum::uint32, Enum::int64, Enum::uint64,
            Enum::double_, Enum::string,  Enum::vec,    Enum::map};
    static char const* toString(Enum e) {
        switch (e) {
        case Enum::null: return "null";
        case Enum::boolean: return "boolean";
        case Enum::char_: return "char";
        case Enum::uchar: return "uchar";
        case Enum::int16: return "int16";
        case Enum::uint16: return "uint16";
        case Enum::int32: return "int32";
        case Enum::uint32: return "uint32";
        case Enum::int64: return "int64";
        case Enum::uint64: return "uint64";
        case Enum::double_: return "double_";
        case Enum::string: return "string";
        case Enum::vec: return "vec";
        case Enum::map: return "map";
        }
        throw std::logic_error(
                "'" + std::to_string(static_cast<std::underlying_type_t<Enum>>(e)) +
                "' is not handled in "
                "EnumTraits<Variant::TypeTag>::toString(Variant::TypeTag)");
    }
};

} // namespace serialize
