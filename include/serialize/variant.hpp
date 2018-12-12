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


// local
#include <serialize/pimpl.hpp>
#include <serialize/meta.hpp>

// 3rd
#include <rapidjson/document.h>

// std
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>


namespace serialize {


///
/// An error identifying `Variant` error
///
class VariantErr : public std::runtime_error {
public:
    explicit VariantErr(std::string const& x) : runtime_error(x) {}
};


///
/// Empty Variant error
///
class VariantEmpty final : public VariantErr {
public:
    explicit VariantEmpty() : VariantErr("Attempt to get empty `Variant`") {}
};


///
/// Bad type error
///
class VariantBadType final : public VariantErr {
public:
    explicit VariantBadType() : VariantErr("Attempt to get wrong type") {}
};


///
/// User tried to get a type which is not able to hold the actual value
///
class VariantIntegralOverflow final : public VariantErr {
public:
    explicit VariantIntegralOverflow(
            std::string const type_name,
            std::string const value)
        : VariantErr("The type '" + type_name +
                     "' can not hold the value '" +
                     value +
                     "'")
    {}
};

///
/// Serialized object reprezentation class
///
class Variant {
public:
    using Map = std::unordered_map<std::string, Variant>;
    using Vec = std::vector<Variant>;

    Variant();
    ~Variant();

    explicit Variant(bool);

    explicit Variant(char);
    explicit Variant(short int);
    explicit Variant(unsigned short int);
    explicit Variant(int);
    explicit Variant(unsigned int);
    explicit Variant(signed long);
    explicit Variant(unsigned long);
    explicit Variant(double);

    explicit Variant(char const*const&);
    explicit Variant(std::string const&);
    explicit Variant(std::string&&);

    explicit Variant(Vec const&);
    explicit Variant(Vec&&);

    explicit Variant(Map const&);
    explicit Variant(Map&&);

    template <typename T,
              typename = decltype(T::toVariant(std::declval<T>()))>
    explicit Variant(T const& x)
        : Variant(T::toVariant(x))
    {}

    template <typename T,
              typename = decltype(T::fromVariant(std::declval<Variant>()))>
    explicit operator T() const {
        return T::fromVariant(*this);
    }

    // copy
    Variant(Variant const& rhs);
    Variant& operator=(Variant const& rhs);

    // move
    Variant(Variant&& rhs) noexcept;
    Variant& operator=(Variant&& rhs) noexcept;

    ///
    /// Get as `T` or `x` if the object is empty
    ///
    template <typename T>
    T asOr(T x) const;

    ///
    /// Get bool
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    bool boolean() const;
    explicit operator bool() const { return boolean(); }

    ///
    /// Get bool or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    bool booleanOr(bool x) const;

    ///
    /// Get char
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    char character() const;
    explicit operator char() const { return character(); }

    ///
    /// Get char or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    char characterOr(char x) const;

    ///
    /// Get short int
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    short int shortInt() const;
    explicit operator short int() const { return shortInt(); }

    ///
    /// Get short int or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    short int shortIntOr(short int x) const;

    ///
    /// Get unsigned short int
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned short int ushortInt() const;
    explicit operator unsigned short int() const { return ushortInt(); }

    ///
    /// Get unsigned short int or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned short int ushortIntOr(unsigned short int) const;

    ///
    /// Get int
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    int integer() const;
    explicit operator int() const { return integer(); }

    ///
    /// Get int or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    int integerOr(int x) const;

    ///
    /// Get unsigned int
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned int uint() const;
    explicit operator unsigned int() const { return uint(); }

    ///
    /// Get unsigned int or `x` if the object is empty
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned int uintOr(unsigned int x) const;

    ///
    /// Get signed long or `x` if the object is empty
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    signed long longInt() const;
    explicit operator signed long() const { return longInt(); }

    ///
    /// Get signed long or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    signed long longInteOr(signed long x) const;

    ///
    /// Get unsigned long or `x` if the object is empty
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned long ulongInt() const;
    explicit operator unsigned long() const { return ulongInt(); }

    ///
    /// Get unsigned long or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    unsigned long ulongIntOr(unsigned long x) const;

    ///
    /// Get int
    /// \throw `VariantEmpty`, `VariantBadType`, `VariantIntegralOverflow`
    ///
    double floating() const;
    explicit operator double() const { return floating(); }

    ///
    /// Get int or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    double floatingOr(double x) const;

    ///
    /// Get string
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    std::string const& str() const;
    explicit operator std::string const&() const { return str(); }

    ///
    /// Get string or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    std::string strOr(std::string const& x) const;

    ///
    /// Get Vec
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    Vec const& vec() const;
    explicit operator Vec const&() const { return vec(); }

    ///
    /// Get Vec or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    Vec vecOr(Vec const& x) const;

    ///
    /// Get Map
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    Map const& map() const;
    explicit operator Map const&() const { return map(); }

    ///
    /// Get Map or `x` if the object is empty
    /// \throw `VariantBadType`, `VariantIntegralOverflow`
    ///
    Map mapOr(Map const& x) const;

    /// \defgroup Variant equality comparison
    /// \{
    bool operator==(Variant const& rhs) const noexcept;
    bool operator!=(Variant const& rhs) const noexcept;
    /// \}

    /// \defgroup Formats Serialized data formats

    /// \defgroup Json
    /// \ingroup Formats
    /// \{
    static Variant from(rapidjson::Value const& json);
    rapidjson::Document& to(rapidjson::Document& json) const;
    /// \}

    ///
    /// Stream operator
    ///
    friend std::ostream& operator<<(std::ostream& os, Variant const& var);

    /// \defgroup type_info
    /// \ingroup type_info
    /// \{
    std::type_info const& typeInfo() const;
    /// \}

    /// List of supported types
    using Types = S<
        bool,
        char,
        short int,
        unsigned short int,
        int,
        unsigned int,
        signed long,
        unsigned long,
        double,
        std::string,
        Variant::Vec,
        Variant::Map>;

private:
    struct Impl;
    Pimpl<Impl> impl;
};


using VariantMap = std::unordered_map<std::string, Variant>;
using VariantVec = std::vector<Variant>;


template <> inline bool Variant::asOr<bool>(bool x) const { return booleanOr(x); }
template <> inline char Variant::asOr<char>(char x) const { return characterOr(x); }
template <> inline short int Variant::asOr<short int>(short int x) const { return shortIntOr(x); }
template <> inline unsigned short int Variant::asOr<unsigned short int>(unsigned short int x) const { return ushortIntOr(x); }
template <> inline int Variant::asOr<int>(int x) const { return integerOr(x); }
template <> inline unsigned int Variant::asOr<unsigned int>(unsigned int x) const { return uintOr(x); }
template <> inline signed long Variant::asOr<signed long>(signed long x) const { return longInteOr(x); }
template <> inline unsigned long Variant::asOr<unsigned long>(unsigned long x) const { return ulongIntOr(x); }


}
