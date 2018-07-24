#pragma once


// std
#include <string_view>


template <class T>
constexpr std::string_view typeName() {
    using namespace std;

#ifdef __clang__
#error "Not supported"

#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
    auto start = p.find("T = ") + 4;
    auto const end = p.find(';', start);
    auto const tmp = p.rfind("::", end);
    if (tmp > start) { start = tmp + 2; }
    return string_view(p.data() + start, end - start);

#elif defined(_MSC_VER)
#error "Not supported"
#endif
}
