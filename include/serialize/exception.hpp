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

#include <serialize/type_name.hpp>

#include <boost/hana/type.hpp>

#include <exception>
#include <string>

namespace serialize {

/// \ingroup group-exceptions
/// An error identifying `Variant` error
class VariantErr : public std::exception {
public:
    explicit VariantErr(std::string const& msg)
        : what_(msg) {}

    char const* what() const noexcept override {
        return what_.c_str();
    }

    void prependPath(std::string const& val) {
        if (path_.empty()) {
            what_ = ": " + what_;
        }
        what_ = "." + val + what_;
        path_ += "." + val + path_;
    }

    std::string const& path() const noexcept { return path_; }

private:
    std::string what_;
    std::string path_;
};

/// \ingroup group-exceptions
/// Empty Variant error
class VariantEmpty final : public VariantErr {
public:
    template <class T>
    explicit VariantEmpty(boost::hana::basic_type<T>)
        : VariantErr("expected '" +
                     std::string(unqualifiedTypeName<std::remove_cv_t<std::remove_reference_t<T>>>()) +
                     "', actual: 'Empty'") {}
};

/// \ingroup goup-exceptions
/// Bad type error
class VariantBadType final : public VariantErr {
public:
    template <class E, class A>
    VariantBadType(boost::hana::basic_type<E>, boost::hana::basic_type<A>)
        : VariantErr(
              "expected '" +
              std::string(unqualifiedTypeName<std::remove_cv_t<std::remove_reference_t<E>>>()) +
              "', actual '" +
              std::string(unqualifiedTypeName<std::remove_cv_t<std::remove_reference_t<A>>>()) +
              "'") {}

    VariantBadType(std::string const& msg) : VariantErr(msg) {}

    template <class T>
    VariantBadType(std::string const& value, boost::hana::basic_type<T>)
        : VariantErr(
              "'" + value + "'" +
              " is not of type '" +
              std::string(unqualifiedTypeName<std::remove_cv_t<std::remove_reference_t<T>>>()) +
              "'") {}
};

/// \ingroup group-exceptions
/// User tried to get a type which is not able to hold the actual value
class VariantIntegralOverflow final : public VariantErr {
public:
    VariantIntegralOverflow(
        std::string const& type_name,
        std::string const& value)
        : VariantErr("The type '" +
                     type_name + "' can not hold the value '" + value + "'") {}
};

} // namespace serialize
