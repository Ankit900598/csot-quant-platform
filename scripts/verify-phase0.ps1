# Week 1 Phase 0 verification on Windows + WSL.
$ErrorActionPreference = "Continue"

$msys = "C:\msys64\ucrt64\bin"
if (Test-Path $msys) { $env:Path = "$msys;" + $env:Path }

Write-Host "`n=== Windows toolchain (MSYS2 ucrt64 on PATH) ===" -ForegroundColor Cyan
@(
  @{ Name = "g++"; Cmd = "g++ --version" },
  @{ Name = "cmake"; Cmd = "cmake --version" },
  @{ Name = "python"; Cmd = "& 'C:\Users\HP\AppData\Local\Programs\Python\Python312\python.exe' --version" },
  @{ Name = "git"; Cmd = "git --version" }
) | ForEach-Object {
  Write-Host ("`n[{0}]" -f $_.Name) -ForegroundColor Yellow
  try { Invoke-Expression $_.Cmd 2>&1 | Select-Object -First 1 } catch { Write-Host "MISSING" }
}

Write-Host "`n=== WSL Ubuntu toolchain ===" -ForegroundColor Cyan
wsl -d Ubuntu -- bash -lc "echo g++: \$(g++ --version 2>/dev/null | head -1 || echo MISSING); echo cmake: \$(cmake --version 2>/dev/null | head -1 || echo MISSING); echo python3: \$(python3 --version 2>/dev/null || echo MISSING); echo perf: \$(perf --version 2>/dev/null | head -1 || echo MISSING); echo valgrind: \$(valgrind --version 2>/dev/null | head -1 || echo MISSING); echo benchmark: \$(dpkg -l libbenchmark-dev 2>/dev/null | tail -1 || echo MISSING)"

Write-Host "`nIf WSL packages show MISSING, run:" -ForegroundColor Green
Write-Host "  wsl -d Ubuntu bash /mnt/c/Users/HP/Projects/csot-quant-platform/scripts/setup-wsl-phase0.sh"
