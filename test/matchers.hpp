/*
  MIT License

  Copyright (c) 2021 Nicolai Trandafil

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

#include <yenxo/exception.hpp>

#include <catch2/catch.hpp>

namespace Catch {

template <>
struct StringMaker<yenxo::VariantBadType> {
    static std::string convert(yenxo::VariantBadType const& value) {
        return std::string("{ what: \"") + value.what() + "\"; path: \"" + value.path()
             + "\" }";
    }
};

} // namespace Catch

template <class T>
struct PathIsMatcher : Catch::MatcherBase<T> {
    explicit PathIsMatcher(std::string path)
            : path(std::move(path)) {
    }

    bool match(T const& in) const override {
        return in.path() == path;
    }

    std::string describe() const override {
        return std::string("has path \"") + path + "\"";
    }

    std::string path;
};

template <class T>
struct ExceptionIsMatcher : Catch::MatcherBase<T> {
    explicit ExceptionIsMatcher(std::string what, std::string path)
            : what(std::move(what))
            , path(std::move(path)) {
    }

    bool match(T const& in) const override {
        return what == in.what() && path == in.path();
    }

    std::string describe() const override {
        return std::string("is ") + "{ what: \"" + what + "\"; path: \"" + path + "\" }";
    }

    std::string what;
    std::string path;
};

template <typename T>
PathIsMatcher<T> PathIs(std::string const& path) {
    return PathIsMatcher<T>{path};
}

template <typename T>
ExceptionIsMatcher<T> ExceptionIs(std::string const& what, std::string const& path) {
    return ExceptionIsMatcher<T>{what, path};
}
