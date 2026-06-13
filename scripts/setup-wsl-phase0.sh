#!/usr/bin/env bash
# Week 1 Phase 0 — install Linux toolchain inside WSL Ubuntu.
#
# If you are ALREADY inside Ubuntu (prompt like ankit@LAPTOP...$), run:
#   bash /mnt/c/Users/HP/Projects/csot-quant-platform/scripts/setup-wsl-phase0.sh
#
# If you are in Windows PowerShell, run:
#   wsl -d Ubuntu bash /mnt/c/Users/HP/Projects/csot-quant-platform/scripts/setup-wsl-phase0.sh

set -euo pipefail

echo "==> Updating apt package lists..."
sudo apt-get update -qq

echo "==> Installing Week 1 toolchain packages (core)..."
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  build-essential \
  cmake \
  git \
  python3 \
  python3-pip \
  python3-venv \
  linux-tools-common \
  linux-tools-generic \
  libbenchmark-dev \
  valgrind \
  kcachegrind

# WSL2 uses a Microsoft kernel — linux-tools-$(uname -r) is often NOT in apt.
# Do not let that block cmake/valgrind/benchmark (the Week 1 checklist items).
KT="linux-tools-$(uname -r)"
echo "==> Optional: kernel-matched perf tools ($KT)..."
if ! sudo DEBIAN_FRONTEND=noninteractive apt-get install -y "$KT" 2>/dev/null; then
  echo "    (skipped — normal on WSL2; will try generic perf symlink below)"
fi

if ! command -v perf >/dev/null 2>&1; then
  PERF_BIN="$(find /usr/lib/linux-tools -name perf -type f 2>/dev/null | head -1 || true)"
  if [[ -n "$PERF_BIN" ]]; then
    echo "==> Linking perf from $PERF_BIN"
    sudo ln -sf "$PERF_BIN" /usr/local/bin/perf
  else
    echo "    WARNING: perf not found. Week 1 code still works; perf numbers may be unavailable on WSL2."
  fi
fi

echo "==> Installing Python packages for Phase 2 experiments (optional plots)..."
python3 -m pip install --user matplotlib

echo "==> Allowing perf for non-root users (Week 1 checklist item)..."
if ! grep -q 'kernel.perf_event_paranoid = 1' /etc/sysctl.d/99-perf.conf 2>/dev/null; then
  echo 'kernel.perf_event_paranoid = 1' | sudo tee /etc/sysctl.d/99-perf.conf >/dev/null
  sudo sysctl --system >/dev/null
fi

echo ""
echo "==> Verification"
g++ --version | head -1
cmake --version | head -1
python3 --version
git --version
if command -v perf >/dev/null 2>&1; then perf --version | head -1; else echo "perf: NOT AVAILABLE (WSL2 limitation)"; fi
valgrind --version | head -1
dpkg -l libbenchmark-dev | tail -1
command -v kcachegrind || true

echo ""
echo "Phase 0 WSL setup complete."
