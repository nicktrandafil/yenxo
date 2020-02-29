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

#include <benchmark/benchmark.h>
#include <variant>

namespace {

template <typename... Args>
struct Overload : public Args... {
    using Args::operator()...;
};

template <typename... Args>
Overload(Args...)->Overload<Args...>;

} // namespace

struct Var {
    std::variant<int, double, long, void*> val;
};
static_assert(sizeof(Var) == 16);

int work(Var var) {
    static int state = 0;
    std::visit(
        Overload{
            [&](int x) { state += x; },
            [&](double x) { state += static_cast<int>(x); },
            [&](long x) { state += static_cast<int>(x); },
            [&](void* x) { state += reinterpret_cast<size_t>(x); }},
        var.val);
    return state;
}

int work2(Var var) {
    static int state = 0;
    switch (var.val.index()) {
    case 0: state += std::get<0>(var.val); break;
    case 1: state += static_cast<int>(std::get<1>(var.val)); break;
    case 2: state += static_cast<int>(std::get<2>(var.val)); break;
    case 3: state += reinterpret_cast<uint64_t>(std::get<3>(var.val)); break;
    case std::variant_npos: break;
    }
    return state;
}

static void bm_set_var(benchmark::State& state) {
    Var var;
    size_t i = 0;
    for (auto s : state) {
        switch ((i++) % 4) {
        case 0: var.val = int(i); break;
        case 1: var.val = double(i); break;
        case 2: var.val = long(i); break;
        case 3: var.val = reinterpret_cast<void*>(i); break;
        }
        benchmark::DoNotOptimize(work(var));
    }
}
BENCHMARK(bm_set_var);

static void bm_set_var2(benchmark::State& state) {
    Var var;
    size_t i = 0;
    for (auto s : state) {
        switch ((i++) % 4) {
        case 0: var.val = int(i); break;
        case 1: var.val = double(i); break;
        case 2: var.val = long(i); break;
        case 3: var.val = reinterpret_cast<void*>(i); break;
        }
        benchmark::DoNotOptimize(work2(var));
    }
}
BENCHMARK(bm_set_var2);


struct Union {
    union {
        int int_v;
        double double_v;
        long long_v;
        void* ptr_v;
    } val;
    uint8_t type{0};
};
static_assert(sizeof(Union) == 16);

int work(Union var) {
    static int state = 0;
    switch (var.type) {
    case 0: break;
    case 1: state += var.val.int_v; break;
    case 2: state += static_cast<int>(var.val.double_v); break;
    case 3: state += static_cast<int>(var.val.long_v); break;
    case 4: state += reinterpret_cast<uint64_t>(var.val.ptr_v); break;
    }
    return state;
}

static void bm_set_union(benchmark::State& state) {
    Union var;
    size_t i = 0;
    for (auto s : state) {
        switch ((i++) % 4) {
        case 0:
            var.val.int_v = int(i);
            var.type = 1;
            break;
        case 1:
            var.val.double_v = double(i);
            var.type = 2;
            break;
        case 2:
            var.val.long_v = long(i);
            var.type = 3;
            break;
        case 3:
            var.val.ptr_v = reinterpret_cast<void*>(i);
            var.type = 4;
            break;
        }
        benchmark::DoNotOptimize(work(var));
    }
}
BENCHMARK(bm_set_union);

BENCHMARK_MAIN();
