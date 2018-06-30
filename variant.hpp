#pragma once


// local
#include <pimpl.hpp>

// std
#include <string>
#include <type_traits>


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
class VariantEmpty : public VariantErr {
public:
    explicit VariantEmpty() : VariantErr("Attempt to get empty `Variant`") {}
};


class VariantBadType : public VariantErr {
public:
    explicit VariantBadType() : VariantErr("Attempt to get wrong type") {}
};

///
/// Serialized object reprezentation class
///
class Variant {
public:
    // forward
    class Map;
    class Vec;

    // Stored type id type
    enum class TypeId {
        No,
        Int,
        Str,
        Vec,
        Map
    };

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

    ///
    /// Get stored type
    ///
    TypeId type() const noexcept;

    ///
    /// Vector type
    ///
    class Vec {
    public:
        Vec();
        ~Vec();

        // copy
        Vec(Vec const& rhs);
        Vec& operator=(Vec const& rhs);

        // move
        Vec(Vec&& rhs) noexcept;
        Vec& operator=(Vec&& rhs) noexcept;

        //
        // std containder compatiable interface
        //

    private:
        struct Impl;
        Pimpl<Impl> impl;
    };

    ///
    /// Associative type (like `std::map`)
    ///
    class Map {
    public:
        Map();
        ~Map();

        // copy
        Map(Map const& rhs);
        Map& operator=(Map const& rhs);

        // move
        Map(Map&& rhs) noexcept;
        Map& operator=(Map&& rhs) noexcept;

        //
        // std containder compatiable interface
        //

    private:
        struct Impl;
        Pimpl<Impl> impl;
    };

private:
    struct Impl;
    Pimpl<Impl> impl;
};
