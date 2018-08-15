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
#include <pimpl.hpp>

// std
#include <string>
#include <vector>
#include <unordered_map>


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

    explicit Variant(short int);
    explicit Variant(unsigned short int);
    explicit Variant(int);
    explicit Variant(unsigned int);
    explicit Variant(double);

    explicit Variant(std::string const&);
    explicit Variant(std::string&&);

    explicit Variant(Vec const&);
    explicit Variant(Vec&&);

    explicit Variant(Map const&);
    explicit Variant(Map&&);

    // copy
    Variant(Variant const& rhs);
    Variant& operator=(Variant const& rhs);

    // move
    Variant(Variant&& rhs) noexcept;
    Variant& operator=(Variant&& rhs) noexcept;

    ///
    /// Get short int
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    short int shortInt() const;
    explicit operator short int() const { return shortInt(); }

    ///
    /// Get short int or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    short int shortIntOr(short int x) const;

    ///
    /// Get unsigned short int
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    unsigned short int ushortInt() const;
    explicit operator unsigned short int() const { return ushortInt(); }

    ///
    /// Get unsigned short int or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    unsigned short int ushortIntOr(unsigned short int) const;

    ///
    /// Get int
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    int integer() const;
    explicit operator int() const { return integer(); }

    ///
    /// Get int or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    int integerOr(int x) const;

    ///
    /// Get unsigned int
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    unsigned int uint() const;
    explicit operator unsigned int() const { return uint(); }

    ///
    /// Get unsigned int or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    unsigned int uintOr(unsigned int x) const;

    ///
    /// Get int
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    double floating() const;
    explicit operator double() const { return floating(); }

    ///
    /// Get int or `x` if the object is empty
    /// \throw `VariantBadType`
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
    /// \throw `VariantBadType`
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
    /// \throw `VariantBadType`
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
    /// \throw `VariantBadType`
    ///
    Map mapOr(Map const& x) const;

    /// \defgroup Variant equality comparison
    /// \{
    bool operator==(Variant const& rhs) const noexcept;
    bool operator!=(Variant const& rhs) const noexcept;
    /// \}

private:
    struct Impl;
    Pimpl<Impl> impl;
};
