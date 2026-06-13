// cache_sim.cpp — Week 2 judge submission (single translation unit).
#include "cache_sim.hpp"

#include <array>
#include <cstdint>
#include <cstring>

namespace {

constexpr int L1_SETS = 64;
constexpr int L1_WAYS = 8;
constexpr int L2_SETS = 512;
constexpr int L2_WAYS = 8;

struct CacheLevel {
    int sets = 0;
    int ways = 0;
    int index_mask = 0;
    int tag_shift = 0;

    std::array<std::uint64_t, 4096> tag{};    // max L2: 512*8
    std::array<std::uint8_t, 4096> valid{};
    std::array<std::uint8_t, 4096> dirty{};
    std::array<std::uint8_t, 4096> lru_pos{};  // 0 = MRU, ways-1 = LRU

    void init(int s, int w, int mask, int shift) {
        sets = s;
        ways = w;
        index_mask = mask;
        tag_shift = shift;
        const std::size_t n = static_cast<std::size_t>(s * w);
        std::memset(valid.data(), 0, n);
        std::memset(dirty.data(), 0, n);
        for (int set = 0; set < s; ++set) {
            for (int way = 0; way < w; ++way) {
                lru_pos[static_cast<std::size_t>(set * w + way)] =
                    static_cast<std::uint8_t>(way);
            }
        }
    }

    [[nodiscard]] int idx(int set, int way) const { return set * ways + way; }

    [[nodiscard]] int find(int set, std::uint64_t t) const {
        const int base = set * ways;
        for (int w = 0; w < ways; ++w) {
            const int i = base + w;
            if (valid[i] && tag[i] == t) {
                return w;
            }
        }
        return -1;
    }

    void touch(int set, int way) {
        const int i = idx(set, way);
        const std::uint8_t old = lru_pos[i];
        const int base = set * ways;
        for (int w = 0; w < ways; ++w) {
            std::uint8_t& p = lru_pos[base + w];
            if (p < old) {
                ++p;
            }
        }
        lru_pos[i] = 0;
    }

    [[nodiscard]] int victim_way(int set) const {
        const int base = set * ways;
        for (int w = 0; w < ways; ++w) {
            if (!valid[base + w]) {
                return w;
            }
        }
        int victim = 0;
        std::uint8_t best = 0;
        for (int w = 0; w < ways; ++w) {
            const std::uint8_t p = lru_pos[base + w];
            if (p >= best) {
                best = p;
                victim = w;
            }
        }
        return victim;
    }

    void set_line(int set, int way, bool v, bool d, std::uint64_t t) {
        const int i = idx(set, way);
        valid[i] = static_cast<std::uint8_t>(v);
        dirty[i] = static_cast<std::uint8_t>(d);
        tag[i] = t;
    }

    bool is_valid(int set, int way) const { return valid[idx(set, way)] != 0; }
    bool is_dirty(int set, int way) const { return dirty[idx(set, way)] != 0; }
    std::uint64_t get_tag(int set, int way) const { return tag[idx(set, way)]; }

    void set_dirty(int set, int way, bool d) {
        dirty[idx(set, way)] = static_cast<std::uint8_t>(d);
    }
};

class CacheSimulator final : public csot::CacheSim {
public:
    void on_init() override {
        l1_.init(L1_SETS, L1_WAYS, 63, 6);
        l2_.init(L2_SETS, L2_WAYS, 511, 9);
    }

    csot::CacheStats run(const csot::MemAccess* acc, std::size_t n) override {
        csot::CacheStats st{};

        for (std::size_t i = 0; i < n; ++i) {
            const std::uint64_t addr = acc[i].address;
            const bool wr = acc[i].is_write != 0;
            if (wr) {
                ++st.writes;
            } else {
                ++st.reads;
            }

            const std::uint64_t b = addr >> 6;
            const int s1 = static_cast<int>(b & 63);
            const std::uint64_t t1 = b >> 6;

            const int w1 = l1_.find(s1, t1);
            if (w1 >= 0) {
                ++st.l1_hits;
                l1_.touch(s1, w1);
                if (wr) {
                    l1_.set_dirty(s1, w1, true);
                }
                continue;
            }

            ++st.l1_misses;

            const int s2 = static_cast<int>(b & 511);
            const std::uint64_t t2 = b >> 9;

            const int w2 = l2_.find(s2, t2);
            if (w2 >= 0) {
                ++st.l2_hits;
                l2_.touch(s2, w2);
            } else {
                ++st.l2_misses;
                const int v = l2_.victim_way(s2);
                if (l2_.is_valid(s2, v) && l2_.is_dirty(s2, v)) {
                    ++st.dirty_writebacks;
                }
                l2_.set_line(s2, v, true, false, t2);
                l2_.touch(s2, v);
            }

            const int v1 = l1_.victim_way(s1);
            if (l1_.is_valid(s1, v1) && l1_.is_dirty(s1, v1)) {
                const std::uint64_t bv = (l1_.get_tag(s1, v1) << 6) | static_cast<std::uint64_t>(s1);
                const int s2v = static_cast<int>(bv & 511);
                const std::uint64_t t2v = bv >> 9;
                const int wv = l2_.find(s2v, t2v);
                if (wv >= 0) {
                    l2_.set_dirty(s2v, wv, true);
                } else {
                    const int vv = l2_.victim_way(s2v);
                    if (l2_.is_valid(s2v, vv) && l2_.is_dirty(s2v, vv)) {
                        ++st.dirty_writebacks;
                    }
                    l2_.set_line(s2v, vv, true, true, t2v);
                    l2_.touch(s2v, vv);
                }
            }

            l1_.set_line(s1, v1, true, wr, t1);
            l1_.touch(s1, v1);
        }

        return st;
    }

private:
    CacheLevel l1_{};
    CacheLevel l2_{};
};

}  // namespace

extern "C" csot::CacheSim* create_cache_sim() {
    return new CacheSimulator();
}
