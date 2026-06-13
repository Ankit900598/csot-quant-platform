# Week 1 — Quant Platform

**Status:** Starter scaffold (curriculum files + sample strategies). Engine implementation in progress.

## Contents

- `include/strategy.hpp`, `histogram.hpp` — frozen ABI
- `samples/spec_strategy.cpp` — reference strategy
- `STRATEGY_SPEC.md` — algorithm spec

## Build (when engine is added)

```bash
cmake -B build && cmake --build build -j
```

## Leaderboard

Build `spec_strategy.so` with judge-portable flags, then upload to https://csot-low-latency.devclub.in/dashboard/
