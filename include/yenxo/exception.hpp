/*
  MIT License

  Copyright (c) 2020 Nicolai Trandafil

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

#include <yenxo/type_name.hpp>

#include <boost/hana/type.hpp>

#include <stdexcept>
#include <string>
#include <string_view>

namespace yenxo {

/// An error identifying `Variant` error
/// \ingroup group-exceptions
class VariantErr : public std::runtime_error {
public:
    explicit VariantErr(std::string const& msg)
            : runtime_error(msg) {
    }

    void prependPath(std::string val) {
        replace_all(val, "~", "~0");
        replace_all(val, "/", "~1");
        path_ = "/" + val + path_;
    }

    std::string const& path() const noexcept {
        return path_;
    }

private:
    static void replace_all(std::string& val,
                            std::string_view old,
                            std::string_view new_) {
        size_t i{};
        while ((i = val.find(old, i)) != std::string::npos) {
            val.replace(i, 1, new_);
            i += new_.size();
        }
    }

    std::string path_;
};

/// Empty Variant error
/// \ingroup group-exceptions
class VariantEmpty final : public VariantErr {
public:
    template <class T>
    explicit VariantEmpty(boost::hana::basic_type<T> t)
            : VariantErr("expected '" + std::string(typeName(t)) + "', actual: 'Empty'") {
    }
};

/// Bad type error
/// \ingroup group-exceptions
class VariantBadType final : public VariantErr {
public:
    template <class E, class A>
    VariantBadType(boost::hana::basic_type<E> e, boost::hana::basic_type<A> a)
            : VariantErr("expected '" + std::string(typeName(e)) + "', actual '"
                         + std::string(typeName(a)) + "'") {
    }

    VariantBadType(std::string const& msg)
            : VariantErr(msg) {
    }

    template <class T>
    VariantBadType(std::string const& value, boost::hana::basic_type<T> t)
            : VariantErr("'" + value + "'" + " is not of type '"
                         + std::string(typeName(t)) + "'") {
    }
};

/// User tried to get a type which is not able to hold the actual value
/// \ingroup group-exceptions
class VariantIntegralOverflow final : public VariantErr {
public:
    VariantIntegralOverflow(std::string const& type_name, std::string const& value)
            : VariantErr("The type '" + type_name + "' can not hold the value '" + value
                         + "'") {
    }
};

/// String conversion error
/// \ingroup group-exceptions
///
/// An error during `toString`/`fromString` conversion
struct StringConversionError : public std::logic_error {
    template <typename T>
    explicit StringConversionError(std::string value, boost::hana::basic_type<T> const&)
            : std::logic_error("'" + value + "'" + " is not of type '"
                               + std::string(typeName(boost::hana::type_c<T>)) + "'") {
    }

    explicit StringConversionError(std::string const& msg)
            : std::logic_error(msg) {
    }
};

} // namespace yenxo
