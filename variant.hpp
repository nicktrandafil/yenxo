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
/// Serialized object reprezentation class
///
class Variant {
public:
    using Map = std::unordered_map<std::string, Variant>;
    using Vec = std::vector<Variant>;

    Variant();
    ~Variant();

    explicit Variant(int);
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
    /// Get integer
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    int integer() const;
    explicit operator int() const { return integer(); }

    ///
    /// Get integer or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    int integerOr(int x) const;

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

    /// \defgroup Variant equality comparation
    /// \{
    bool operator==(Variant const& rhs) const noexcept;
    bool operator!=(Variant const& rhs) const noexcept;
    /// \}

private:
    struct Impl;
    Pimpl<Impl> impl;
};
