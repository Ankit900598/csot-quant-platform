# Week 1 Phase 0 — Setup Learning Notes (Ankit)

This file is the **only** place in this repo with teaching/explanation content for setup.
The rest of the repo stays clean project code from the CSoT curriculum.

## What we are building

Week 1 goal: a C++ quant platform that replays market ticks, runs a strategy, measures latency, and later submits to the leaderboard.

Reference curriculum: [CSOT-26-Low-Latency](https://github.com/itxprashant/CSOT-26-Low-Latency.git) → `week-1/`.

## Your machine layout

| Layer | Role |
|-------|------|
| **Windows** | Daily dev in Cursor, git, Python |
| **MSYS2 g++** | C++ compiler on Windows (`C:\msys64\ucrt64\bin\g++.exe`) |
| **WSL2 Ubuntu** | Linux-only tools: `perf`, `valgrind`, `libbenchmark-dev` |

Why two environments? Week 1 profiling tools (`perf`, Cachegrind) are Linux-native. The course expects Ubuntu. WSL2 is the standard Windows workaround.

## Phase 0 checklist (from curriculum)

1. `g++` ≥ 11
2. `cmake` ≥ 3.20
3. `python3` ≥ 3.8
4. `git`
5. `perf` (Linux)
6. `valgrind` (Linux)
7. Google Benchmark (`libbenchmark-dev` on Ubuntu)
8. `kcachegrind` (optional, for Callgrind GUI)
9. Fresh git repo + `.gitignore`
10. `perf` non-root permission (`kernel.perf_event_paranoid = 1`)

## What was installed automatically

### On Windows (done by agent)

- **CMake 4.3.3** via winget (`Kitware.CMake`) → `C:\Program Files\CMake\bin\cmake.exe`
- **CMake 4.2.1** via MSYS2 (pairs with MSYS2 g++; use this for Week 1 builds)
- **Project repo** at `C:\Users\HP\Projects\csot-quant-platform`
- **Starter files** copied from `week-1/project/` (strategy ABI, histogram, CMake template, data generator, specs)

Already present before setup:

- **g++ 15.2** (MSYS2)
- **Python 3.12**
- **Git 2.53**
- **matplotlib** (Python 3.12)
- **Google Benchmark 1.9.4** (MSYS2 `mingw-w64-ucrt-x86_64-benchmark`)
- **WSL2 Ubuntu** (running)

### On WSL (you run once — needs sudo password)

```powershell
wsl -d Ubuntu bash /mnt/c/Users/HP/Projects/csot-quant-platform/scripts/setup-wsl-phase0.sh
```

That script installs: `build-essential`, `cmake`, `perf`, `valgrind`, `libbenchmark-dev`, `kcachegrind`, and Python `matplotlib` for optional plots.

## Verify everything

```powershell
powershell -ExecutionPolicy Bypass -File C:\Users\HP\Projects\csot-quant-platform\scripts\verify-phase0.ps1
```

## Create GitHub repo (one-time)

`gh` was not logged in on your machine. Run:

```powershell
gh auth login
cd C:\Users\HP\Projects\csot-quant-platform
git add .
git commit -m "Week 1 Phase 0: starter project and toolchain setup"
gh repo create csot-quant-platform --public --source=. --remote=origin --push
```

Use your GitHub username instead of the repo name if you prefer `ankit-csot-week1`.

## Why each library exists

| Tool | Why Week 1 needs it |
|------|---------------------|
| **g++ / clang++** | C++20 strategy + engine |
| **CMake** | Build runner, `.so` strategy, benchmarks |
| **Python 3** | `data/gen.py` tick generator; optional matplotlib plots |
| **Google Benchmark** | Microbenchmark hot functions correctly |
| **perf** | Real hardware counters (IPC, cache misses) |
| **Valgrind** | Memory correctness (leaks, UB) |
| **Cachegrind** | Deterministic cache miss analysis |
| **kcachegrind** | Visualize Callgrind output |

## Phase order (this chat = Week 1 only)

| Phase | What | Status |
|-------|------|--------|
| **0** | Environment setup | Windows done; WSL — run script below |
| **1** | Read topic notes `01`–`04` (+ bonus `05`) | Next |
| **2** | Hands-on demos (vector vs list, perf, benchmark) | After reading |
| **3** | Build quant platform project | Main work |
| **4** | Push repo + leaderboard submission | End of week |

## Quick quiz (after Phase 0)

1. Why is `perf` installed in WSL but not Windows?
2. What does `libbenchmark-dev` give you that `std::chrono` alone does not?
3. Why do we `.gitignore` `synthetic_large.csv`?

<details>
<summary>Answers</summary>

1. `perf` uses Linux kernel performance counters; Windows has different profilers (VTune, etc.).
2. Google Benchmark handles warm-up, multiple iterations, preventing dead-code elimination (`DoNotOptimize`), and reports throughput.
3. 10M-row CSV is huge; regenerate locally with `python3 data/gen.py`.

</details>
