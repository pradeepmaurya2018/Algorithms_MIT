# NebulaKV – Distributed Key-Value Store (C++)

A high-performance **distributed in-memory key-value database** implemented in C++ using Linux system primitives.
The system demonstrates core infrastructure concepts including event-driven networking, persistence, replication, cluster routing, and consistent hashing.

This project is designed as a **systems engineering learning project** that mirrors architectural ideas used in real-world data systems such as Redis and distributed messaging platforms.

---

## Features

* Event-driven TCP server using **epoll**
* In-memory key-value storage
* Command protocol (`SET`, `GET`, `DEL`)
* Append-only persistence log
* Replication support
* Cluster routing
* Consistent hashing
* Benchmark client for load testing
* Metrics endpoint for observability

---

## Architecture

```
client
   ↓
TCP server (epoll event loop)
   ↓
command parser
   ↓
in-memory storage engine
   ↓
persistence (append-only log)
   ↓
replication layer
   ↓
cluster routing
   ↓
consistent hashing
```

Each component is implemented as a separate module to maintain a clean systems-oriented architecture.

---

## Project Structure

```
kvstore/
 ├── src/
 │   ├── main.cpp
 │   ├── server.cpp
 │   ├── parser.cpp
 │   ├── storage.cpp
 │   ├── persistence.cpp
 │   ├── replication.cpp
 │   └── cluster.cpp
 │
 ├── include/
 │   ├── server.h
 │   ├── parser.h
 │   ├── storage.h
 │   ├── persistence.h
 │   ├── replication.h
 │   └── cluster.h
 │
 ├── benchmark/
 │   └── benchmark.cpp
 │
 ├── kvlog.txt
 └── README.md
```

---

## Build

Compile the server:

```
g++ src/*.cpp -Iinclude -o kvstore
```

Compile the benchmark client:

```
g++ benchmark/benchmark.cpp -o bench -lpthread
```

---

## Running the Server

Start the server:

```
./kvstore
```

The server listens on:

```
localhost:6379
```

---

## Basic Usage

Connect using `netcat`:

```
nc localhost 6379
```

Example session:

```
SET name pradeep
OK

GET name
pradeep

DEL name
OK
```

---

## Persistence

All write operations are appended to a log file:

```
kvlog.txt
```

On restart, the server replays the log to rebuild the in-memory state.

---

## Replication

Replication uses a **primary–replica model**.

```
client
   ↓
primary node
   ↓
replica nodes
```

Write operations received by the primary node are forwarded to replica nodes.

---

## Cluster Routing

In cluster mode, keys are distributed across multiple nodes.

```
hash(key) → node
```

Nodes are selected using **consistent hashing**, ensuring minimal redistribution when nodes join or leave the cluster.

---

## Benchmarking

Run the benchmark client:

```
./bench
```

The benchmark generates concurrent requests to measure:

* requests per second
* connection throughput
* latency under load

---

## Metrics

The server exposes runtime metrics through a simple endpoint.

Example metrics output:

```
requests_total 10234
connections_total 310
```

These metrics can be integrated with monitoring systems such as Prometheus.

---

## Learning Objectives

This project demonstrates several important systems engineering concepts:

* Linux socket programming
* Event-driven architecture using epoll
* Storage engine fundamentals
* Append-only persistence
* Replication protocols
* Consistent hashing
* Distributed system design

---

## Future Improvements

Potential extensions:

* multi-threaded worker model
* asynchronous disk persistence
* Raft-based consensus
* sharding and rebalancing
* HTTP API interface
* containerized deployment

---

## License

MIT License
