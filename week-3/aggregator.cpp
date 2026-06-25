#include "aggregate.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>

#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#endif

namespace {

inline void merge_row(csot::SymbolAgg& dst, const csot::SymbolAgg& src) {
    if (src.count == 0) {
        return;
    }
    if (dst.count == 0) {
        dst = src;
        return;
    }
    dst.count += src.count;
    dst.sum_price += src.sum_price;
    dst.sum_qty += src.sum_qty;
    if (src.min_price < dst.min_price) {
        dst.min_price = src.min_price;
    }
    if (src.max_price > dst.max_price) {
        dst.max_price = src.max_price;
    }
}

inline void accumulate_range(const csot::AggTick* ticks, std::size_t begin,
                             std::size_t end, csot::SymbolAgg* partial,
                             std::uint32_t num_symbols) {
    std::memset(partial, 0,
                static_cast<std::size_t>(num_symbols) * sizeof(csot::SymbolAgg));

    for (std::size_t i = begin; i < end; ++i) {
        const csot::AggTick& t = ticks[i];
        csot::SymbolAgg& r = partial[t.symbol_id];
        if (r.count == 0) {
            r.min_price = t.price;
            r.max_price = t.price;
        } else {
            if (t.price < r.min_price) {
                r.min_price = t.price;
            }
            if (t.price > r.max_price) {
                r.max_price = t.price;
            }
        }
        r.count += 1;
        r.sum_price += t.price;
        r.sum_qty += t.qty;
    }
}

#if defined(__linux__)
inline void pin_to_core(int core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}
#endif

struct alignas(64) PartialTable {
    std::vector<csot::SymbolAgg> rows;
};

class ParallelAggregator final : public csot::Aggregator {
    static constexpr int kThreads = 4;

    std::uint32_t num_symbols_ = 0;
    std::array<PartialTable, kThreads> partials_{};
    std::array<std::thread, kThreads> workers_{};

public:
    void on_init(std::uint32_t num_symbols) override {
        num_symbols_ = num_symbols;
        for (auto& partial : partials_) {
            partial.rows.assign(num_symbols, csot::SymbolAgg{0, 0, 0, 0, 0});
        }
    }

    void run(const csot::AggTick* ticks, std::size_t n,
             csot::SymbolAgg* out) override {
        for (std::uint32_t s = 0; s < num_symbols_; ++s) {
            out[s] = csot::SymbolAgg{0, 0, 0, 0, 0};
        }

        if (n == 0) {
            return;
        }

        const std::size_t chunk = (n + kThreads - 1) / kThreads;
        int active = 0;
        for (int t = 0; t < kThreads; ++t) {
            const std::size_t begin = static_cast<std::size_t>(t) * chunk;
            if (begin >= n) {
                break;
            }
            const std::size_t end = std::min(begin + chunk, n);
            const int thread_id = t;
            workers_[active++] = std::thread([this, ticks, begin, end, thread_id]() {
#if defined(__linux__)
                pin_to_core(thread_id);
#endif
                accumulate_range(ticks, begin, end, partials_[thread_id].rows.data(),
                                 num_symbols_);
            });
        }

        for (int t = 0; t < active; ++t) {
            workers_[t].join();
        }

        for (int t = 0; t < active; ++t) {
            const csot::SymbolAgg* partial = partials_[t].rows.data();
            for (std::uint32_t s = 0; s < num_symbols_; ++s) {
                merge_row(out[s], partial[s]);
            }
        }
    }
};

}  // namespace

extern "C" csot::Aggregator* create_aggregator() {
    return new ParallelAggregator();
}
