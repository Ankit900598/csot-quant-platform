#!/usr/bin/env bash
# Week 1 Phase 0 — install Linux toolchain inside WSL Ubuntu.
# Run once from PowerShell:
#   wsl -d Ubuntu bash /mnt/c/Users/HP/Projects/csot-quant-platform/scripts/setup-wsl-phase0.sh

set -euo pipefail

echo "==> Updating apt package lists..."
sudo apt-get update -qq

echo "==> Installing Week 1 toolchain packages..."
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  build-essential \
  cmake \
  git \
  python3 \
  python3-pip \
  python3-venv \
  linux-tools-common \
  linux-tools-generic \
  "linux-tools-$(uname -r)" \
  libbenchmark-dev \
  valgrind \
  kcachegrind

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
perf --version | head -1
valgrind --version | head -1
dpkg -l libbenchmark-dev | tail -1
command -v kcachegrind || true

echo ""
echo "Phase 0 WSL setup complete."
