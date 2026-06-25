# Week 3 — Parallel Tick Aggregator

**Status:** Complete — `aggregator.cpp` (parallel map-reduce with per-thread partials + merge).

## Build

```bash
cd week-3
cmake -B build-judge -DCSOT_JUDGE_BUILD=ON -DCSOT_AGG_SRC=aggregator.cpp
cmake --build build-judge -j
./build-judge/agg_runner data/tiny.ticks 2>/dev/null | diff - data/tiny.agg.json
```

## Leaderboard

Upload **`week-3/aggregator.cpp`** to https://csot-low-latency.devclub.in/dashboard/

## Implementation notes

- Stream partitioned across up to 4 threads (judge has 4 vCPUs)
- Each thread accumulates into its own partial table (no shared writes)
- Linux: threads pinned with `pthread_setaffinity_np`
- Final merge handles empty partial rows per AGG_SPEC §7
