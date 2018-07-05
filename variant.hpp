#pragma once


// local
#include <pimpl.hpp>

// boost
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/reverse_iterator.hpp>

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


///
/// Bad type error
///
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
    /// Get integer or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    int integerOr(int x) const;

    ///
    /// Get string
    /// \throw `VariantEmpty`, `VariantBadType`
    ///
    std::string const& str() const;

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

    ///
    /// Get Map or `x` if the object is empty
    /// \throw `VariantBadType`
    ///
    Map mapOr(Map const& x) const;

    ///
    /// Get stored type
    ///
    TypeId type() const noexcept;

    /// \defgroup Variant equality comparation
    /// \{
    bool operator==(Variant const& rhs) const noexcept;
    bool operator!=(Variant const& rhs) const noexcept;
    /// \}

    ///
    /// Vector type
    ///
    class Vec {
    public:
        //
        // std containder compatiable interface
        //

        template <typename Value>
        class Iterator : public boost::iterator_facade<
                Iterator<Value>,
                Value,
                std::random_access_iterator_tag> {
        public:
            Iterator();
            ~Iterator();

            template <typename OtherValue,
                      typename = std::enable_if_t<
                          std::is_const_v<Value> &&
                              !std::is_const_v<OtherValue>,
                          void>>
            Iterator(Iterator<OtherValue> const& rhs);

            // copy
            Iterator(Iterator const& rhs);
            Iterator& operator=(Iterator const& rhs);

            // move
            Iterator(Iterator&& rhs) noexcept;
            Iterator& operator=(Iterator&& rhs) noexcept;

        private:
            // `boost::iterator_facade` interface
            typename Iterator::reference dereference() const noexcept;
            bool equal(Iterator const& rhs) const noexcept;
            void increment() noexcept;
            void decrement() noexcept;
            void advance(typename Iterator::difference_type n) noexcept;
            typename Iterator::difference_type distance_to(
                    Iterator const& x) noexcept;

        private:
            // `boost::iterator_facade` requirement
            friend class boost::iterator_core_access;
            template <typename> friend class Iterator;
            friend class Vec;

            struct Impl;
            Pimpl<Impl> impl;

            Iterator(Impl&& impl);
        };

        /// \defgroup Member types
        using value_type = Variant;
        using allocator_type = std::allocator<Variant>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = allocator_type::reference;
        using const_reference = allocator_type::const_reference;
        using pointer = allocator_type::pointer;
        using const_pointer = allocator_type::const_pointer;
        using iterator = Iterator<Variant>;
        using const_iterator = Iterator<Variant const>;
        using reverse_iterator = boost::reverse_iterator<iterator>;
        using const_reverse_iterator = boost::reverse_iterator<const_iterator>;
        /// \}

        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;

        /// \defgroup Iterators
        /// \{
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;

        reverse_iterator rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept;
        /// \}

        /// \defgroup Capacity
        /// \{
        size_type size() const noexcept;
        void resize(size_type n);
        void resize(size_type n, value_type const& val);
        bool empty() const noexcept;
        /// \}

        /// \defgroup Element access
        /// \{
        reference operator[](size_type n) noexcept;
        const_reference operator[](size_type n) const noexcept;
        reference at(size_type n);
        const_reference at(size_type n) const;
        reference front() noexcept;
        const_reference front() const noexcept;
        reference back() noexcept;
        const_reference back() const noexcept;
        /// \}

        /// \defgroup Modifiers
        /// \{
        void push_back(value_type const& val);
        void push_back(value_type&& val);
        void pop_back();
        iterator insert(const_iterator position, value_type const& val);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        void swap(Vec& x) noexcept;
        void clear() noexcept;
        /// \}

        //
        // Interface
        //

        Vec();
        Vec(std::initializer_list<value_type> il);

        ~Vec();

        // copy
        Vec(Vec const& rhs);
        Vec& operator=(Vec const& rhs);

        // move
        Vec(Vec&& rhs) noexcept;
        Vec& operator=(Vec&& rhs) noexcept;

        /// \defgroup Vec equality comparation
        /// \{
        bool operator==(Vec const& rhs) const noexcept;
        bool operator!=(Vec const& rhs) const noexcept;
        /// \}

    private:
        struct Impl;
        Pimpl<Impl> impl;
    };

    ///
    /// Associative type (like `std::unordered_map`)
    ///
    class Map {
    public:
        //
        // std containder compatiable interface
        //

        template <typename Value>
        class Iterator : public boost::iterator_facade<
                Iterator<Value>,
                Value,
                std::forward_iterator_tag> {
        public:
            Iterator();
            ~Iterator();

            template <typename OtherValue,
                      typename = std::enable_if_t<
                          std::is_const_v<Value> &&
                              !std::is_const_v<OtherValue>,
                          void>>
            Iterator(Iterator<OtherValue> const& rhs);

            // copy
            Iterator(Iterator const& rhs);
            Iterator& operator=(Iterator const& rhs);

            // move
            Iterator(Iterator&& rhs) noexcept;
            Iterator& operator=(Iterator&& rhs) noexcept;

        private:
            // `boost::iterator_facade` interface
            typename Iterator::reference dereference() const noexcept;
            bool equal(Iterator const& rhs) const noexcept;
            void increment() noexcept;

        private:
            // `boost::iterator_facade` requirement
            friend class boost::iterator_core_access;
            template <typename> friend class Iterator;
            friend class Map;

            struct Impl;
            Pimpl<Impl> impl;

            Iterator(Impl&& impl);
        };

        using key_type = std::string;
        using mapped_type = Variant;
        using value_type = std::pair<key_type const, mapped_type>;
        using hasher = std::hash<key_type>;
        using key_equal = std::equal_to<key_type>;
        using allocator_type = std::allocator<value_type>;
        using size_type = std::size_t;
        using reference = allocator_type::reference;
        using const_reference = allocator_type::const_reference;
        using pointer = allocator_type::pointer;
        using const_pointer = allocator_type::const_pointer;
        using iterator = Iterator<value_type>;
        using const_iterator = Iterator<value_type const>;

        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;

        /// \defgroup Iterators
        /// \{
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;
        /// \}

        /// \defgroup Capacity
        /// \{
        size_type size() const noexcept;
        bool empty() const noexcept;
        /// \}

        /// \defgroup Element access
        /// \{
        mapped_type& operator[](key_type const& key);
        mapped_type& operator[](key_type&& key);
        mapped_type& at(key_type const& key);
        mapped_type const& at(key_type const& key) const;
        /// \}

        /// \defgroup Modifiers
        /// \{
        std::pair<iterator, bool> insert(value_type const& val);
        std::pair<iterator, bool> insert(value_type&& val);
        iterator erase(iterator position);
        iterator erase(const_iterator first, const_iterator last);
        size_type erase(key_type const& key);
        void swap(Map& x) noexcept;
        void clear() noexcept;
        /// \}

        //
        // Interface
        //

        Map();
        Map(std::initializer_list<value_type> il);

        ~Map();

        // copy
        Map(Map const& rhs);
        Map& operator=(Map const& rhs);

        // move
        Map(Map&& rhs) noexcept;
        Map& operator=(Map&& rhs) noexcept;

        /// \defgroup Map equality comparation
        /// \{
        bool operator==(Map const& rhs) const noexcept;
        bool operator!=(Map const& rhs) const noexcept;
        /// \}

    private:
        struct Impl;
        Pimpl<Impl> impl;
    };

private:
    struct Impl;
    Pimpl<Impl> impl;
};
