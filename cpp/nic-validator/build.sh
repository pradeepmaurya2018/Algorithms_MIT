#!/usr/bin/env bash
set -euo pipefail
cmake -S . -B build
cmake --build build -j"$(nproc)"
echo
echo "Built: ./build/nic-validator"
