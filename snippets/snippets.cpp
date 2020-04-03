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

#include <serialize/variant.hpp>

#include <rapidjson/document.h>

#include <benchmark/benchmark.h>
#include <variant>

using namespace serialize;

struct Var {
    std::variant<int, double, long, void*> val;
};
static_assert(sizeof(Var) == 16);

int work(Var var) {
    static int state = 0;
    std::visit(Overload{[&](int x) { state += x; },
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
    std::array<char, 7> padding;
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

struct X {
    bool operator==(X const& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(X const& rhs) const {
        return !(*this == rhs);
    }
    int x;
    int y;
};

struct X2 {
    bool operator==(X2 const& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(X2 const& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
    int x;
    int y;
};

static void bm_not_equal_using_equal(benchmark::State& state) {
    X x{5, 6};
    X y{6, 7};
    for (auto _ : state) { benchmark::DoNotOptimize(x != y); }
}
BENCHMARK(bm_not_equal_using_equal);

static void bm_not_equal(benchmark::State& state) {
    X2 x{5, 6};
    X2 y{6, 7};
    for (auto _ : state) { benchmark::DoNotOptimize(x != y); }
}
BENCHMARK(bm_not_equal);

static void bm_var_default_construct(benchmark::State& state) {
    for (auto _ : state) {
        Variant var;
        benchmark::DoNotOptimize(var);
    }
}
BENCHMARK(bm_var_default_construct);

static void bm_var_copy_construct(benchmark::State& state) {
    for (auto _ : state) {
        Variant var;
        benchmark::DoNotOptimize(var);
        Variant var2(var);
        benchmark::DoNotOptimize(var2);
    }
}
BENCHMARK(bm_var_copy_construct);

static void bm_var_move_construct(benchmark::State& state) {
    for (auto _ : state) {
        Variant var;
        benchmark::DoNotOptimize(var);
        Variant var2(std::move(var));
        benchmark::DoNotOptimize(var2);
    }
}
BENCHMARK(bm_var_move_construct);

static void bm_var_from_json(benchmark::State& state) {
    auto const raw = R"({
        "x": 6,
        "y": [1, 2],
        "z": {
            "a": "a",
            "b": "b"
        },
        "a": null
    })";

    for (auto _ : state) {
        auto var = serialize::Variant::fromJson(raw);
        benchmark::DoNotOptimize(var);
    }
}
BENCHMARK(bm_var_from_json);

static void bm_var_rj_json(benchmark::State& state) {
    auto const raw = R"({
        "x": 6,
        "y": [1, 2],
        "z": {
            "a": "a",
            "b": "b"
        },
        "a": null
    })";

    for (auto _ : state) {
        rapidjson::Document doc;
        doc.Parse(raw);
        benchmark::DoNotOptimize(doc);
    }
}
BENCHMARK(bm_var_rj_json);

BENCHMARK_MAIN();
