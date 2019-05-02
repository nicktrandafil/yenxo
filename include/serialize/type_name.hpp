/*
  MIT License

  Copyright (c) 2018 Nicolai Trandafil

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


// local
#include <serialize/when.hpp>

// std
#include <string>
#include <string_view>


namespace serialize {


template <class T, class = void>
struct UnqualifiedTypeNameImpl : UnqualifiedTypeNameImpl<T, When<true>> {};


template <class T>
struct UnqualifiedTypeNameT {
    constexpr auto operator()() const;
};


/// Get unqualified type name
template <class T>
constexpr UnqualifiedTypeNameT<T> unqualifiedTypeName;


namespace detail {


constexpr std::string_view getT(std::string_view p) {
    using namespace std;
#if defined(__clang__)
    auto start = p.find("T = ") + 4;
    auto const end = p.find("]", start);
    auto const tmp = p.rfind("::", end);
    if (tmp != std::string_view::npos && tmp > start) { start = tmp + 2; }
    return string_view(p.data() + start, end - start);
#elif defined(__GNUC__)
    auto start = p.find("T = ") + 4;
    auto const end = p.find(";", start);
    auto const tmp = p.rfind("::", end);
    if (tmp != std::string_view::npos && tmp > start) { start = tmp + 2; }
    return string_view(p.data() + start, end - start);
#elif defined(_MSC_VER)
#error "Not supported"
#endif
}


constexpr std::string_view getTempl(std::string_view p) {
#if defined(__clang__)
    auto start = p.find("T = ") + 4;
    auto const end = p.find("<", start);
    auto const tmp = p.rfind("::", end);
    if (tmp != std::string_view::npos && tmp > start) { start = tmp + 2; }
    return std::string_view(p.data() + start, end - start);
#elif defined(__GNUC__)
    return detail::getT(p);
#else
#error "Not supported"
#endif
}


template <auto value>
std::string getVal() {
    auto const pf = std::string_view(__PRETTY_FUNCTION__);
#if defined(__clang__)
    auto start = pf.find("value = ") + 8;
    auto const end = pf.find("]");
    auto tmp = pf.rfind("::", pf.rfind("::") - 1);
    if (tmp != std::string_view::npos && tmp > start) {  start = tmp + 2; }
    return std::string(pf.data() + start, end - start);
#elif defined(__GNUC__)
    auto const end = pf.find(";");
    auto const start = pf.rfind(")", end) + 1;
    return std::string(unqualifiedTypeName<decltype(value)>()) +
            "::" +
            std::string(pf.data() + start, end - start);
#else
#error "Not supported"
#endif
}


}


template <class T, bool condition>
struct UnqualifiedTypeNameImpl<T, When<condition>> {
    static constexpr std::string_view apply() {
        return detail::getT(__PRETTY_FUNCTION__);
    }
};


template <>
struct UnqualifiedTypeNameImpl<std::string> {
    static constexpr std::string_view apply() {
        return "string";
    }
};


template <template <auto> class T, auto v>
struct UnqualifiedTypeNameImpl<T<v>> {
    static std::string apply() {
        using namespace std;
        auto const templ = detail::getTempl(__PRETTY_FUNCTION__);
        return string(templ) + "<" + detail::getVal<v>() + ">";
    }
};


template <template <class...> class T, class... Args>
struct UnqualifiedTypeNameImpl<T<Args...>> {
    static std::string apply() {
        using namespace std;
        string const templ(detail::getTempl(__PRETTY_FUNCTION__));
        string args = ((string(unqualifiedTypeName<Args>()) + ", ") + ...);
        if (!args.empty()) { args.erase(args.end() - 2, args.end()); }
        return templ + "<" + args + ">";
    }
};


template <class T>
constexpr auto UnqualifiedTypeNameT<T>::operator()() const {
    return UnqualifiedTypeNameImpl<T>::apply();
}


/// Get unqualified type name
template <class T>
constexpr std::string_view qualifiedTypeName() {
    using namespace std;
#if defined(__clang__)
    string_view p = __PRETTY_FUNCTION__;
    auto const start = p.find("T = ") + 4;
    auto const end = p.find("]", start);
    return string_view(p.data() + start, end - start);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
    auto const start = p.find("T = ") + 4;
    auto const end = p.find(";", start);
    return string_view(p.data() + start, end - start);
#elif defined(_MSC_VER)
#error "Not supported"
#endif
}


}
