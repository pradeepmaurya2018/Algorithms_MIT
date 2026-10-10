# NIC Validation Framework

Phase 1 of a reusable Linux NIC validation framework.

Current target:
- Realtek RTL8168/8168H
- Fedora Linux
- PCI device `0000:02:00.0`

## Current functionality

The first implementation provides:

- C++20 test framework
- test result/status model
- thread-safe logger
- test runner
- Linux PCI/sysfs NIC discovery backend
- PCI vendor/device/revision discovery
- PCI BAR discovery
- interface/driver discovery
- MAC address discovery
- first PCI discovery testcase

The framework deliberately does **not** directly mmap PCI BARs or modify hardware in Phase 1.

Linux exposes PCI device information and PCI resources under `/sys/bus/pci/devices/<BDF>/`, including `vendor`, `device`, `revision`, `resource`, and `resource0..N`.

## Build

```bash
sudo dnf install gcc-c++ cmake make
cd nic-validator
cmake -S . -B build
cmake --build build -j$(nproc)
```

## Run

For your NIC:

```bash
sudo ./build/nic-validator --test PCI_DISCOVERY --pci 0000:02:00.0
```

Or:

```bash
sudo ./build/nic-validator --all --pci 0000:02:00.0
```

## Expected device

For the currently known RTL8168:

```text
PCI address : 0000:02:00.0
Vendor      : 0x10ec
Device      : 0x8168
Revision    : 0x15
BAR0        : 0x3000 / 0x100 bytes / IO
BAR2        : 0x81204000 / 0x1000 bytes / MMIO
BAR4        : 0x81200000 / 0x4000 bytes / MMIO
```

## Roadmap

1. PCI discovery
2. PCI configuration validation
3. Register access
4. Reset
5. Link
6. MAC
7. Packet generator
8. RX/TX
9. DMA
10. Interrupts
11. Offloads
12. Error injection/recovery
13. Performance
14. Stress
15. Production/fab mode
