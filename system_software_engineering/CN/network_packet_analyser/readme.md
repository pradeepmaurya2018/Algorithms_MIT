# High-Performance NIC Packet Analyzer

A **Linux systems programming project** that captures packets directly from a Network Interface Card (NIC), analyzes flows, and monitors hardware-level NIC statistics.
The project demonstrates **low-level networking, kernel interaction, and high-performance packet processing** techniques used in modern datacenter infrastructure.

The analyzer uses **zero-copy packet capture (PACKET_MMAP)** and multi-threaded processing to achieve high packet throughput while collecting traffic statistics and NIC error counters.

---

## Overview

This project is a minimal **network observability and NIC validation tool**.
It allows engineers to inspect live network traffic and detect hardware-level problems such as packet drops or buffer overflows.

The program captures packets directly from the NIC, parses network headers, tracks active network flows, and displays real-time statistics.

Key goals of the project:

* Demonstrate **Linux kernel packet capture**
* Show how NIC hardware interacts with user-space applications
* Provide insight into **high-performance networking systems**
* Build a simplified foundation similar to production tools used in networking infrastructure

---

## Features

### High-Performance Packet Capture

Captures packets using Linux **PACKET_MMAP** zero-copy ring buffers.

Benefits:

* Avoids expensive kernel-to-user memory copies
* Processes millions of packets per second
* Lower latency and CPU overhead

### Multi-Threaded Packet Processing

Packets are processed across multiple CPU cores.

Each worker thread:

* Reads packets from the capture ring
* Parses packet headers
* Updates flow statistics

### Network Flow Tracking

Tracks traffic using the standard **5-tuple flow definition**:

* Source IP
* Destination IP
* Source Port
* Destination Port
* Protocol

Flow statistics include:

* Packet count
* Byte count

### NIC Hardware Monitoring

Collects hardware counters using the Linux tool **ethtool**.

These counters expose driver and hardware statistics such as:

* `rx_dropped`
* `tx_dropped`
* `rx_errors`
* `tx_errors`

This is useful for **NIC validation and performance debugging**.

### Real-Time Statistics

The analyzer prints live metrics including:

* Packets received
* Total bytes processed
* Active flows
* NIC error counters

---

## System Architecture

```
Network Wire
     ↓
NIC Hardware
     ↓
Linux Driver
     ↓
PACKET_MMAP Ring Buffer
     ↓
Packet Capture Threads
     ↓
Packet Parser
     ↓
Flow Table
     ↓
Statistics Engine
     ↓
Console Output
```

---

## Project Structure

```
network_packet_analyzer/
│
├── main.cpp              # Program entry point
├── packet_ring.cpp       # Zero-copy packet capture
├── packet_ring.h
│
├── packet_parser.cpp     # Ethernet/IP/TCP/UDP parsing
├── packet_parser.h
│
├── flow_table.cpp        # Flow tracking engine
├── flow_table.h
│
├── stats.cpp             # Packet/byte counters
├── stats.h
│
├── nic_stats.cpp         # NIC hardware statistics
├── nic_stats.h
│
└── Makefile
```

---

## Requirements

Linux environment (tested on Ubuntu)

Required packages:

```
build-essential
ethtool
```

Install dependencies:

```bash
sudo apt update
sudo apt install build-essential ethtool
```

---

## Build Instructions

Clone the repository:

```bash
git clone <repo-url>
cd network_packet_analyzer
```

Compile the project:

```bash
make
```

The build produces:

```
nic_analyzer
```

---

## Running the Analyzer

Packet capture requires raw socket privileges.

Run with root:

```bash
sudo ./nic_analyzer <interface>
```

Example:

```bash
sudo ./nic_analyzer enp3s0
```

---

## Finding Your Network Interface

List network interfaces:

```bash
ip link
```

Typical output:

```
1: lo
2: enp3s0
3: wlp2s0
```

Choose the interface receiving traffic.

---

## Example Output

```
NIC Packet Analyzer
Interface: enp3s0
Worker threads: 12

===== Statistics =====
Packets: 2,430,000
Bytes: 1.8GB

===== Top Flows =====
Active flows: 18
Packets: 34,000 Bytes: 42MB
Packets: 21,000 Bytes: 25MB

===== NIC Hardware Counters =====
rx_dropped : 0
tx_dropped : 0
```

---

## Generating Test Traffic

Use tools like:

```
ping
curl
iperf3
```

Example:

```bash
ping google.com
```

or stress test with:

```bash
iperf3 -c <server_ip> -P 8
```

---

## Debugging Tips

Check NIC traffic:

```bash
ip -s link show <interface>
```

Inspect NIC hardware counters:

```bash
sudo ethtool -S <interface>
```

Verify packet capture using:

```bash
sudo tcpdump -i <interface>
```

---

## Learning Outcomes

This project demonstrates practical systems programming concepts:

* Raw packet sockets
* Linux PACKET_MMAP
* Zero-copy packet processing
* Multi-threaded networking
* Flow tracking algorithms
* NIC hardware statistics
* Linux networking internals

---

## Possible Future Improvements

* Receive Side Scaling (RSS) queue analysis
* CPU affinity for packet threads
* eBPF packet filtering
* XDP fast-path packet processing
* Prometheus metrics exporter
* Web-based monitoring dashboard

---

## License

This project is intended for educational and research purposes.

---

## Author

Systems programming and networking experiment exploring **high-performance packet processing on Linux**.
