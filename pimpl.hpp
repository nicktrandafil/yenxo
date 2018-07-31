#pragma once


// std
#include <memory>
#include <type_traits>


///
/// Generic 'Pimpl' idiom implementation
///
/// Throwing member functions do throw if and only if `T` or allocation throws
///
/// Example
/// -------
///
/// x.hpp
/// -----
///
/// #include <pimpl.hpp>
///
/// class X {
/// 	X();
/// 	~X();
///
/// 	X(X const&);
/// 	X& operator=(X const&);
///
/// 	X(X&&);
/// 	X& operator=(X&&);
/// private:
/// 	struct Impl;
/// 	Pimpl<Impl> impl;
/// };
///
/// x.cpp
/// -----
///
/// #include "x.hpp"
/// #include <pimpl_impl.hpp>
///
/// struct X::Impl {};
/// X::X() = default;
/// X::~X() = default;
/// X::X(X const&) = default;
/// X& X::operator=(X const&) = default;
/// X::X(X&&) = default;
/// X& X::operator=(X&&) = default;
///
template <typename T>
class Pimpl {
public:
    Pimpl();
    ~Pimpl();

    // copy

    Pimpl(Pimpl const& rhs);
    // \post Strong exception guarantee
    Pimpl& operator=(Pimpl const& rhs);

    // move
    Pimpl(Pimpl&&) noexcept;
    Pimpl& operator=(Pimpl&&) noexcept;

    //
    // Agregation initialization
    // SFINAE trick to not confuse with copy constructor
    //

    template <typename U,
              typename =
                  typename std::enable_if<
                      !std::is_same<Pimpl, U>::value, void>::type>
    explicit Pimpl(U&& u);

    template <typename T1, typename T2, typename ...Args>
    Pimpl(T1&& a1, T2&& a2, Args&&... args);

    //
    // Dereferencing
    //

    T* operator->() noexcept;
    T const* operator->() const noexcept;

    T& operator*() noexcept;
    T const& operator*() const noexcept;

private:
    std::unique_ptr<T> m;
};
