# Hardware-Aware Network Software Engineer — Coding Problems

## Quick Priority List

| Difficulty | Problem |
|---|---|
| 🟢 Easy | Implement `memcpy()` |
| 🟢 Easy | Implement `memset()` |
| 🟢 Easy | Bit manipulation and register macros |
| 🟡 Medium | Circular buffer / ring buffer |
| 🟡 Medium | Thread-safe queue |
| 🟡 Medium | Memory pool allocator |
| 🟡 Medium | Producer-consumer with `pthreads` |
| 🔴 Hard | Lock-free ring buffer |
| 🔴 Hard | Packet buffer management |
| 🔴 Hard | Concurrent hash table |

---

# 🟢 Easy

| # | Problem | Main Skill |
|---:|---|---|
| 1 | Reverse a singly linked list | Pointers |
| 2 | Detect linked-list cycle | Pointers + DSA |
| 3 | Binary search / variants | Algorithms |
| 4 | Set / clear / toggle / test bits | Bit manipulation |
| 5 | Implement `memcpy()` / `memmove()` | Pointers + memory |

**Goal:** These are fundamentals. You should be able to implement them almost automatically.

---

# 🟡 Medium

| # | Problem | Main Skill |
|---:|---|---|
| 6 | Implement a hash table | Data structures + memory |
| 7 | Implement an LRU cache | Hash map + linked list |
| 8 | Implement a memory pool / allocator | Memory management |
| 9 | Calculate IPv4 checksum | Networking + bit operations |
| 10 | Ethernet / IP / TCP packet parser | C structures + networking |
| 11 | Implement a trie | Data structures |
| 12 | Longest-prefix IP match | Trie / radix tree + networking |

**Goal:** Apply algorithms and data structures to real networking problems.

---

# 🟠 Hard

| # | Problem | Main Skill |
|---:|---|---|
| 13 | Circular buffer / ring buffer | Memory + indexing |
| 14 | Producer-consumer | Threads + synchronization |
| 15 | Thread-safe bounded queue | Mutex + condition variables |
| 16 | Token-bucket rate limiter | Concurrency + networking |
| 17 | Packet / flow table | Hashing + networking + concurrency |

**Goal:** Understand race conditions, ownership, synchronization, and performance.

---

# 🔴 Very Hard

| # | Problem | Main Skill |
|---:|---|---|
| 18 | SPSC lock-free queue | Atomics + memory ordering |
| 19 | NIC descriptor ring / RX-TX simulation | DMA + descriptors + concurrency |
| 20 | Find / fix complex C/C++ memory or pointer bug | Debugging + systems expertise |

**Goal:** Understand the interaction between software, CPU, memory, concurrency, and hardware.

---

# 🎯 Recommended Preparation Order

## 1. C Fundamentals

- Pointers
- `memcpy()` / `memmove()`
- Bit manipulation
- Binary search

↓

## 2. Data Structures + Networking

- Hash table
- LRU cache
- Memory pool
- Packet parser
- Trie
- Longest-prefix match

↓

## 3. Concurrency + Packet Processing

- Ring buffer
- Producer-consumer
- Thread-safe queue
- Token-bucket rate limiter
- Packet / flow table

↓

## 4. High-Performance Systems

- Lock-free queue
- Packet buffer management
- NIC descriptor ring
- DMA
- Memory ordering

---

# ⭐ Highest-Priority Problems

If interview preparation time is limited, prioritize these:

1. Ring buffer
2. Producer-consumer
3. Thread-safe queue
4. Memory pool
5. Packet parser
6. Hash table
7. Longest-prefix match
8. Packet / flow table
9. SPSC lock-free queue
10. NIC descriptor ring / RX-TX simulation

---

# 🚀 Skill Progression

C
→ Pointers
→ Memory
→ Bit Manipulation
→ Data Structures
→ Networking
→ Concurrency
→ Lock-Free Programming
→ Packet Processing
→ DMA
→ NIC Architecture
→ Hardware-Aware Network Software