#include "variant.hpp"

// local
#include <meta.hpp>
#include <pimpl_impl.hpp>
#include <vector>

// std
#include <unordered_map>
#include <variant>


using Val = std::variant<
    std::monostate,
    int,
    std::string,
    Variant::Vec,
    Variant::Map>;


struct Variant::Impl {
    Val m;
};


Variant::Variant() = default;


Variant::~Variant() = default;


Variant::Variant(int x)
    : impl(x)
{}


Variant::Variant(std::string const& x)
    : impl(x)
{}


Variant::Variant(std::string&& x)
    : impl(std::move(x))
{}


Variant::Variant(Vec const& x)
    : impl(x)
{}


Variant::Variant(Vec&& x)
    : impl(std::move(x))
{}


Variant::Variant(Map const& x)
    : impl(x)
{}


Variant::Variant(Map&& x)
    : impl(std::move(x))
{}


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
        throw VariantEmpty();
    }
    template <typename U>
    [[noreturn]] T const& operator()(U const&) const { throw VariantBadType(); }
};


template <typename T>
struct GetHelper<T, When<std::is_integral_v<T>>> {
    T operator()(T x) const noexcept { return x; }
    [[noreturn]]  T operator()(std::monostate) const { throw VariantEmpty(); }
    template <typename U>
    [[noreturn]] T operator()(U) const { throw VariantBadType(); }
};


template <typename T>
struct GetOrHelper {
    T operator()(T x) const { return x; }
    T operator()(std::monostate) const { return t; }
    template <typename U>
    [[noreturn]] T operator()(U const&) const { throw VariantBadType(); }
    T const& t;
};


} // namespace


int Variant::integer() const {
    return std::visit(GetHelper<int>(), impl->m);
}


int Variant::integerOr(int x) const {
    return std::visit(GetOrHelper<int>{x}, impl->m);
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


Variant::Map const& Variant::map() const {
    return std::visit(GetHelper<Map>(), impl->m);
}


Variant::Map Variant::mapOr(Map const& x) const {
    return std::visit(GetOrHelper<Map>{x}, impl->m);
}


bool Variant::operator==(Variant const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


bool Variant::operator!=(Variant const& rhs) const noexcept {
    return impl->m != rhs.impl->m;
}


struct Variant::Vec::Impl {
    std::vector<Variant> m;
};


Variant::Vec::Vec() = default;


Variant::Vec::Vec(std::initializer_list<value_type> il)
    : impl(std::move(il))
{}


Variant::Vec::~Vec() = default;


Variant::Vec::Vec(Vec const&) = default;


Variant::Vec& Variant::Vec::operator=(Vec const&) = default;


Variant::Vec::Vec(Vec&&) noexcept = default;


Variant::Vec& Variant::Vec::operator=(Vec&&) noexcept = default;


template <typename Value>
struct Variant::Vec::Iterator<Value>::Impl {
    using UnderlyingType = std::vector<Variant>;
    using UnderlyingIterator =
        std::conditional_t<std::is_const_v<Value>,
            typename UnderlyingType::const_iterator,
            typename UnderlyingType::iterator>;

    UnderlyingIterator m;
};


template <typename Value>
Variant::Vec::Iterator<Value>::Iterator() = default;


template <typename Value>
Variant::Vec::Iterator<Value>::~Iterator() = default;


template <typename Value>
template <typename OtherValue, typename>
Variant::Vec::Iterator<Value>::Iterator(Iterator<OtherValue> const& rhs)
    : impl(rhs.impl->m)
{}


template <typename Value>
Variant::Vec::Iterator<Value>::Iterator(Impl&& impl)
    : impl(std::move(impl))
{}


template <typename Value>
Variant::Vec::Iterator<Value>::Iterator(Iterator const&) = default;


template <typename Value>
Variant::Vec::Iterator<Value>&
    Variant::Vec::Iterator<Value>::operator=(Iterator const&) = default;


template <typename Value>
Variant::Vec::Iterator<Value>::Iterator(Iterator&&) noexcept = default;


template <typename Value>
Variant::Vec::Iterator<Value>&
    Variant::Vec::Iterator<Value>::operator=(Iterator&&) noexcept = default;


template <typename Value>
bool Variant::Vec::Iterator<Value>::equal(Iterator const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


template <typename Value>
void Variant::Vec::Iterator<Value>::increment() noexcept {
    ++impl->m;
}


template <typename Value>
void Variant::Vec::Iterator<Value>::decrement() noexcept {
    --impl->m;
}


template <typename Value>
void Variant::Vec::Iterator<Value>::advance(
        typename Iterator::difference_type n) noexcept {
    impl->m += n;
}


template <typename Value>
typename Variant::Vec::Iterator<Value>::reference
        Variant::Vec::Iterator<Value>::dereference() const noexcept {
    return *impl->m;
}


template <typename Value>
typename Variant::Vec::Iterator<Value>::difference_type
        Variant::Vec::Iterator<Value>::distance_to(Iterator const& x) noexcept {
    return std::distance(impl->m, x.impl->m);
}


Variant::Vec::iterator Variant::Vec::begin() noexcept {
    return iterator::Impl{impl->m.begin()};
}


Variant::Vec::const_iterator Variant::Vec::begin() const noexcept {
    return const_iterator::Impl{impl->m.begin()};
}


Variant::Vec::const_iterator Variant::Vec::cbegin() const noexcept {
    return const_iterator::Impl{impl->m.cbegin()};
}


Variant::Vec::iterator Variant::Vec::end() noexcept {
    return iterator::Impl{impl->m.end()};
}


Variant::Vec::const_iterator Variant::Vec::end() const noexcept {
    return const_iterator::Impl{impl->m.end()};
}


Variant::Vec::const_iterator Variant::Vec::cend() const noexcept {
    return const_iterator::Impl{impl->m.cend()};
}


Variant::Vec::reverse_iterator Variant::Vec::rbegin() noexcept {
    return reverse_iterator(begin());
}


Variant::Vec::const_reverse_iterator Variant::Vec::rbegin() const noexcept {
    return const_reverse_iterator(rbegin());
}


Variant::Vec::const_reverse_iterator Variant::Vec::crbegin() const noexcept {
    return const_reverse_iterator(crbegin());
}


Variant::Vec::reverse_iterator Variant::Vec::rend() noexcept {
    return reverse_iterator(end());
}


Variant::Vec::const_reverse_iterator Variant::Vec::rend() const noexcept {
    return const_reverse_iterator(end());
}


Variant::Vec::const_reverse_iterator Variant::Vec::crend() const noexcept {
    return const_reverse_iterator(cend());
}


Variant::Vec::size_type Variant::Vec::size() const noexcept {
    return impl->m.size();
}


void Variant::Vec::resize(size_type n) {
    impl->m.resize(n);
}


void Variant::Vec::resize(size_type n, value_type const& val) {
    impl->m.resize(n, val);
}


bool Variant::Vec::empty() const noexcept {
    return impl->m.empty();
}


Variant::Vec::reference Variant::Vec::operator[](size_type n) noexcept {
    return impl->m[n];
}


Variant::Vec::const_reference Variant::Vec::operator[](
        size_type n) const noexcept {
    return impl->m[n];
}


Variant::Vec::reference Variant::Vec::at(size_type n) {
    return impl->m.at(n);
}


Variant::Vec::const_reference Variant::Vec::at(size_type n) const {
    return impl->m.at(n);
}


Variant::Vec::reference Variant::Vec::front() noexcept {
    return impl->m.front();
}


Variant::Vec::const_reference Variant::Vec::front() const noexcept {
    return impl->m.front();
}


Variant::Vec::reference Variant::Vec::back() noexcept {
    return impl->m.back();
}


Variant::Vec::const_reference Variant::Vec::back() const noexcept {
    return impl->m.back();
}


void Variant::Vec::push_back(value_type const& val) {
    impl->m.push_back(val);
}


void Variant::Vec::push_back(value_type&& val) {
    impl->m.push_back(std::move(val));
}


void Variant::Vec::pop_back() {
    impl->m.pop_back();
}


Variant::Vec::iterator Variant::Vec::insert(const_iterator position,
                                            value_type const& val) {
    return iterator::Impl{impl->m.insert(position.impl->m, val)};
}


Variant::Vec::iterator Variant::Vec::erase(iterator position) {
    return iterator::Impl{impl->m.erase(position.impl->m)};
}


Variant::Vec::iterator Variant::Vec::erase(iterator first, iterator last) {
    return iterator::Impl{impl->m.erase(first.impl->m, last.impl->m)};
}


void Variant::Vec::swap(Vec& x) noexcept {
    std::swap(impl, x.impl);
}


void Variant::Vec::clear() noexcept {
    impl->m.clear();
}


bool Variant::Vec::operator==(Vec const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


bool Variant::Vec::operator!=(Vec const& rhs) const noexcept {
    return impl->m != rhs.impl->m;
}


// explicit instantiation
template class Variant::Vec::Iterator<Variant>;
template class Variant::Vec::Iterator<Variant const>;
template Variant::Vec::Iterator<Variant const>::Iterator<Variant>(
        Iterator<Variant> const&);


struct Variant::Map::Impl {
    std::unordered_map<std::string, Variant> m;
};


Variant::Map::Map() = default;


Variant::Map::Map(std::initializer_list<value_type> il)
    : impl(std::move(il))
{}


Variant::Map::~Map() = default;


Variant::Map::Map(Map const&) = default;


Variant::Map& Variant::Map::operator=(Map const&) = default;


Variant::Map::Map(Map&&) noexcept = default;


Variant::Map& Variant::Map::operator=(Map&&) noexcept = default;


template <typename Value>
struct Variant::Map::Iterator<Value>::Impl {
    using UnderlyingType = std::unordered_map<std::string, Variant>;
    using UnderlyingIterator =
        std::conditional_t<std::is_const_v<Value>,
            typename UnderlyingType::const_iterator,
            typename UnderlyingType::iterator>;

    UnderlyingIterator m;
};


template <typename Value>
Variant::Map::Iterator<Value>::Iterator() = default;


template <typename Value>
Variant::Map::Iterator<Value>::~Iterator() = default;


template <typename Value>
template <typename OtherValue, typename>
Variant::Map::Iterator<Value>::Iterator(Iterator<OtherValue> const& rhs)
    : impl(rhs.impl->m)
{}


template <typename Value>
Variant::Map::Iterator<Value>::Iterator(Impl&& impl)
    : impl(std::move(impl))
{}


template <typename Value>
Variant::Map::Iterator<Value>::Iterator(Iterator const&) = default;


template <typename Value>
Variant::Map::Iterator<Value>&
    Variant::Map::Iterator<Value>::operator=(Iterator const&) = default;


template <typename Value>
Variant::Map::Iterator<Value>::Iterator(Iterator&&) noexcept = default;


template <typename Value>
Variant::Map::Iterator<Value>&
    Variant::Map::Iterator<Value>::operator=(Iterator&&) noexcept = default;


template <typename Value>
bool Variant::Map::Iterator<Value>::equal(Iterator const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


template <typename Value>
typename Variant::Map::Iterator<Value>::reference
        Variant::Map::Iterator<Value>::dereference() const noexcept {
    return *impl->m;
}


template <typename Value>
void Variant::Map::Iterator<Value>::increment() noexcept {
    ++impl->m;
}


Variant::Map::iterator Variant::Map::begin() noexcept {
    return iterator::Impl{impl->m.begin()};
}


Variant::Map::const_iterator Variant::Map::begin() const noexcept {
    return const_iterator::Impl{impl->m.begin()};
}


Variant::Map::const_iterator Variant::Map::cbegin() const noexcept {
    return const_iterator::Impl{impl->m.cbegin()};
}


Variant::Map::iterator Variant::Map::end() noexcept {
    return iterator::Impl{impl->m.end()};
}


Variant::Map::const_iterator Variant::Map::end() const noexcept {
    return const_iterator::Impl{impl->m.end()};
}


Variant::Map::const_iterator Variant::Map::cend() const noexcept {
    return const_iterator::Impl{impl->m.cend()};
}


Variant::Map::size_type Variant::Map::size() const noexcept {
    return impl->m.size();
}


bool Variant::Map::empty() const noexcept {
    return impl->m.empty();
}


Variant::Map::mapped_type& Variant::Map::operator[](key_type const& key) {
    return impl->m[key];
}


Variant::Map::mapped_type& Variant::Map::operator[](key_type&& key) {
    return impl->m[std::move(key)];
}


Variant::Map::mapped_type& Variant::Map::at(key_type const& key) {
    return impl->m.at(key);
}


Variant::Map::mapped_type const& Variant::Map::at(key_type const& key) const {
    return impl->m.at(key);
}


std::pair<Variant::Map::iterator, bool> Variant::Map::insert(
        value_type const& val) {
    auto const [it, suc] = impl->m.insert(val);
    return std::pair(iterator(iterator::Impl{it}), suc);
}


std::pair<Variant::Map::iterator, bool> Variant::Map::insert(value_type&& val) {
    auto const [it, suc] = impl->m.insert(std::move(val));
    return std::pair(iterator(iterator::Impl{it}), suc);
}


Variant::Map::iterator Variant::Map::erase(iterator position) {
    return iterator::Impl{impl->m.erase(position.impl->m)};
}


Variant::Map::iterator Variant::Map::erase(
        const_iterator first, const_iterator last) {
    return iterator::Impl{impl->m.erase(first.impl->m, last.impl->m)};
}


Variant::Map::size_type Variant::Map::erase(key_type const& key) {
    return impl->m.erase(key);
}


void Variant::Map::swap(Map& x) noexcept {
    std::swap(impl, x.impl);
}


void Variant::Map::clear() noexcept {
    impl->m.clear();
}


bool Variant::Map::operator==(Map const& rhs) const noexcept {
    return impl->m == rhs.impl->m;
}


bool Variant::Map::operator!=(Map const& rhs) const noexcept {
    return impl->m != rhs.impl->m;
}


// explicit instantiation
template class Variant::Map::Iterator<Variant::Map::value_type>;
template class Variant::Map::Iterator<Variant::Map::value_type const>;
template Variant::Map::Iterator<Variant::Map::value_type const>::
    Iterator<Variant::Map::value_type>(
        Iterator<Variant::Map::value_type> const&);
