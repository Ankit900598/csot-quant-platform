# Week 2 — Cache Simulator (Ankit Yadav)

## Build

```bash
cmake -B build-judge -DCSOT_JUDGE_BUILD=ON -DCSOT_CACHE_SIM_SRC=cache_sim.cpp
cmake --build build-judge -j
```

## Run

```bash
./build-judge/cache_sim_runner data/tiny.trace
python3 data/gen_trace.py --accesses 5000000 --seed 42 --out data/large.trace
./build-judge/cache_sim_runner data/large.trace
```

## Correctness

`data/tiny.trace` matches `data/tiny.stats.json` (all seven counters).

## Hardware

WSL2 Ubuntu 24.04, g++ 13.3.0, Intel laptop (Windows host).

## Results (judge flags, 5M-access trace, seed 42)

| Metric | Value |
|--------|-------|
| `run()` wall-clock | 240,617,370 ns |
| Throughput | 20.78 M acc/s |

## Submit

Upload `cache_sim.cpp` to https://csot-low-latency.devclub.in/dashboard/
