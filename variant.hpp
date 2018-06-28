#pragma once


// local
#include <pimpl.hpp>

// std
#include <string>
#include <type_traits>


class Variant {
public:
    // Stored type id type
    enum class TypeId {
        No,
        Int,
        Str,
        Vec,
        Map
    };

    Variant();

    explicit Variant(int);
    explicit Variant(std::string const&);
    explicit Variant(std::string&&);

    // Construction from a type. Disable for Variant type
    template <typename T,
              typename = std::enable_if_t<
                  !std::is_same<Variant, T>::value, void>>
    explicit Variant(T&& x);

    // copy
    Variant(Variant const& rhs);
    Variant& operator=(Variant const& rhs);

    // move
    Variant(Variant&& rhs) noexcept;
    Variant& operator=(Variant&& rhs) noexcept;

    /// Get stored type
    TypeId type() const noexcept;

    // Vector type
    class Vec {
    public:
        Vec();
        ~Vec();

        // copy
        Vec(Vec const& rhs);
        Vec& operator=(Vec const& rhs);

        // move
        Vec(Vec&& rhs);
        Vec& operator=(Vec&& rhs);

        //
        // std containder compatiable interface
        //

    private:
        struct Impl;
        Pimpl<Impl> impl;
    };

    // Associative type (like `std::map`)
    class Map {
    public:
        Map();
        ~Map();

        // copy
        Map(Map const& rhs);
        Map& operator=(Map const& rhs);

        // move
        Map(Map&& rhs);
        Map& operator=(Map&& rhs);

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
