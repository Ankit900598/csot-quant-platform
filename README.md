# csot-quant-platform

**Ankit Yadav** — CSoT'26 Low Latency Track  
Curriculum: [CSOT-26-Low-Latency](https://github.com/itxprashant/CSOT-26-Low-Latency)  
Leaderboard: https://csot-low-latency.devclub.in/dashboard/

## Submission status

| Week | Folder | Repo | Leaderboard upload |
|------|--------|------|-------------------|
| 1 | [`week-1/`](week-1/) | Starter scaffold + samples | `spec_strategy.so` (build locally) |
| 2 | [`week-2/`](week-2/) | **Complete** — `cache_sim.cpp` | Upload `week-2/cache_sim.cpp` |
| 3 | [`week-3/`](week-3/) | Not started | TBD |
| 4 | [`week-4/`](week-4/) | Not started | TBD |
| 5 | [`week-5/`](week-5/) | Not started | TBD |

## Layout

```
csot-quant-platform/
├── week-1/     # quant strategy platform
├── week-2/     # cache simulator
├── week-3/     # placeholder
├── week-4/     # placeholder
├── week-5/     # placeholder
└── scripts/    # toolchain setup
```

Each week folder is self-contained (`CMakeLists.txt`, `include/`, `data/`).

## Quick start

**Week 2 (ready to submit):**

```bash
cd week-2
cmake -B build-judge -DCSOT_JUDGE_BUILD=ON -DCSOT_CACHE_SIM_SRC=cache_sim.cpp
cmake --build build-judge -j
./build-judge/cache_sim_runner data/tiny.trace
```

## Submit this repo

Share this URL in the CSoT submission form / group:

**https://github.com/Ankit900598/csot-quant-platform**

Leaderboard uploads (`.so` / `.cpp`) are separate — see each week's README.
