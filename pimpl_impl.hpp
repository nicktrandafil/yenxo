#pragma once


template <typename T>
Pimpl<T>::Pimpl() : m(new T) {}


template <typename T>
Pimpl<T>::~Pimpl() = default;


template <typename T>
Pimpl<T>::Pimpl(Pimpl const& rhs) : m(new T(*rhs.m)) {}


template <typename T>
Pimpl<T>::Pimpl(Pimpl&& rhs) = default;


template <typename T>
Pimpl<T>& Pimpl<T>::operator=(Pimpl const& rhs) {
    Pimpl<T> tmp(rhs);
    *this = std::move(tmp);
    return *this;
}


template <typename T>
Pimpl<T>& Pimpl<T>::operator=(Pimpl<T>&& rhs) = default;


template <typename T>
template <typename U, typename>
Pimpl<T>::Pimpl(U&& u) : m(new T{std::forward<U>(u)}) {}


template <typename T>
template <typename U1, typename U2, typename ...Args>
Pimpl<T>::Pimpl(U1&& u1, U2&& u2, Args&&... args)
    : m(new T{std::forward<U1>(u1),
              std::forward<U2>(u2),
              std::forward<Args>(args)...
            })
{}


template <typename T>
T* Pimpl<T>::operator->() { return m.get(); }


template <typename T>
T const* Pimpl<T>::operator->() const { return m.get(); }


template <typename T>
T& Pimpl<T>::operator*() { return *m; }


template <typename T>
T const& Pimpl<T>::operator*() const { return *m; }
