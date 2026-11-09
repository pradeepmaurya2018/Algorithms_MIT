# IPU Simulator — Validation Framework Development Wiki

**Document type:** Engineering wiki / development guide  
**Applies to:** `ipu-simulator` C++20 project  
**Status:** Proposed framework architecture and practices  
**Audience:** Developers, validation engineers, reviewers and contributors

> **Purpose.** Define how to turn the IPU simulator into a repeatable, requirements-driven validation environment. The framework should not merely show that a packet can traverse the simulator. It should provide evidence that modeled hardware blocks and their interactions behave according to explicit requirements—including boundary conditions, resource pressure, failures, recovery and performance.

---

## 1. What validation means in this project

Validation is a disciplined experiment:

1. State a requirement or behavior to be demonstrated.
2. Put the device under a known configuration.
3. Apply a controlled stimulus.
4. Observe the device at defined boundaries.
5. Compare observations with an independently derived expected result.
6. Record the outcome, evidence, coverage and any failure.

A test that only checks that `process_packet()` returned `true` is usually not enough. A useful test may also need to check packet bytes, metadata, descriptor state, buffer ownership, counters, selected egress port, drop reason and resource cleanup.

### 1.1 Verification, validation and testing

These terms overlap in day-to-day engineering, but they describe different questions:

| Term | Main question | Example in this project |
|---|---|---|
| Verification | Does an implementation conform to its specification? | Check that the Ethernet parser extracts the destination MAC and EtherType at the specified offsets. |
| Validation | Does the modeled device satisfy its intended behavior and use cases? | Exercise an RX-to-TX packet journey and verify the expected forwarding behavior and resource lifecycle. |
| Testing | The activity used to obtain evidence. | Submit a packet, inspect the result and compare it to an oracle. |

The project is a **software model**. Passing its tests establishes confidence in the model and its specified behavior; it does not prove electrical, timing, signal-integrity, PCIe-PHY, silicon or firmware properties that the model does not represent.

### 1.2 What a good validation result proves

For each scenario, the framework should make it possible to answer:

- Which requirement was tested?
- Which exact configuration and stimulus were used?
- What was expected, and why?
- What was actually observed?
- At which stage did behavior differ?
- Were packets, descriptors and buffers left in a valid state?
- Can the failure be reproduced from a seed and test configuration?
- Which requirements and coverage points have been exercised?

---

## 2. Project context and design boundary

The repository structure supplied for this wiki already contains these areas:

- `include/core/` and `src/core/` — custom test framework and runner.
- `include/ipu/` and `src/ipu/` — IPU device and port abstractions.
- `include/hardware/` and `src/hardware/` — modeled hardware subsystems.
- `include/tests/` and `src/tests/` — simulator test cases.
- `tests/hardware/` — lower-level hardware component tests.

The hardware directories include datapath pipeline, Ethernet MAC/parser, IPv4/IPv6 parsers, L2 forwarding, packet buffers, packet I/O, RX/TX descriptor rings and VLAN.

**Integration rule:** extend the existing test runner and naming conventions where practical. Do not introduce a second test framework, a second packet model or a parallel device abstraction without a concrete need. The code examples in this wiki are architectural examples; names such as `TestContext`, `PacketFactory`, `Scoreboard` and `FaultInjector` are proposed concepts, not claims about existing APIs.

### 2.1 The modeled device under test (DUT)

The DUT is the IPU simulator and the hardware blocks it composes. Keep a clear boundary between:

- **DUT behavior:** modeled queues, descriptors, packet buffers, parser, classification, forwarding and transmit behavior.
- **Testbench behavior:** packet generation, configuration, fault controls, monitors, reference calculations and assertions.
- **Framework behavior:** registration, execution, isolation, timeout, logging, result aggregation and reporting.

The testbench may configure and observe the DUT through documented interfaces. It should not reach into private implementation details just to make an assertion pass.

---

## 3. Core development principles

### Principle 1 — Requirements first, tests second

Every important test should trace to a requirement, design rule or explicitly identified exploratory objective. Avoid accumulating tests whose purpose is unclear.

A requirement should be specific and measurable. Prefer:

> For a configured RX queue with an available descriptor and a valid frame that fits the posted buffer, the simulator shall complete the descriptor with the received frame length and preserve the frame bytes.

Avoid:

> RX should work.

### Principle 2 — Define the oracle before writing the test

An **oracle** is the independent source of expected behavior. It may be a specification rule, a small reference model, a fixed expected byte sequence, a state invariant or a mathematical calculation.

Do not calculate expected results by calling the same DUT function that produces the actual result. That creates a self-confirming test.

### Principle 3 — Separate stimulus, DUT, observation and checking

Keep four jobs distinct:

1. **Stimulus:** create frames, configure ports, submit descriptors and trigger events.
2. **DUT:** perform the modeled device operation.
3. **Observation:** capture completions, counters, packet bytes, state transitions and events.
4. **Checking:** compare observations with the oracle.

This separation makes failures easier to diagnose and allows the same stimulus to be reused with different checkers.

### Principle 4 — Test at multiple levels

Use small, fast tests for individual components and a smaller number of end-to-end tests for integration. Do not use a full datapath test to diagnose every parser edge case.

### Principle 5 — Make tests deterministic and replayable

A test should produce the same result for the same build, configuration and seed, unless it is explicitly testing concurrency or nondeterministic scheduling. Record random seeds, packet parameters and fault settings.

### Principle 6 — Assert invariants, not only final output

Check that the system remains internally consistent. Examples:

- A buffer is not owned by two components at the same time.
- A descriptor is not completed twice.
- A ring's producer/consumer distance never exceeds its capacity.
- A packet dropped at a stage does not appear at a later stage.
- A buffer is not recycled while a packet still references it.
- Reset returns documented state to its reset values.

### Principle 7 — Fail safely and recover

For each injected failure, verify both the immediate error response and the system's ability to continue or recover. A test that observes a drop but ignores leaked descriptors is incomplete.

### Principle 8 — Tests must be isolated

Each test starts from a known device state and cleans up resources even when an assertion fails. Avoid shared mutable global state and order-dependent tests.

### Principle 9 — Use realistic boundaries

Exercise minimum and maximum supported values, one below and one above the boundary, ring wraparound, empty/full states, and transitions between states. Most subtle defects occur at boundaries.

### Principle 10 — Keep the framework smaller than the system it tests

Prefer a few transparent abstractions over a large generic test DSL. Add abstraction only when it removes duplication or makes an important validation concept clearer.

### Principle 11 — Report evidence, not just PASS/FAIL

A useful failure includes the test ID, requirement ID, seed, configuration, expected value, actual value, stage, packet or descriptor identifier, and relevant counters/logs.

### Principle 12 — Do not confuse simulation with silicon validation

Name capabilities honestly. If the simulator models DMA as a copy/event/state transition, call it a **simulated DMA transfer**. Do not claim that it validates real PCIe transactions, IOMMU behavior, bus ordering or electrical timing unless those are explicitly modeled and separately evidenced.

---

## 4. Overall framework architecture

The recommended conceptual architecture is:

```text
                REQUIREMENTS / TEST PLAN
                           |
                           v
                  TEST CASE REGISTRY
                           |
                           v
+----------------------------------------------------+
|                 VALIDATION TESTBENCH               |
|                                                    |
|  Configuration       Packet / Traffic Generator    |
|  Fault Controller    Reference Model / Scoreboard  |
|  Monitors            Assertions / Invariant Checks |
|  Coverage Collector  Evidence / Event Capture      |
+-----------------------------+----------------------+
                              |
                              v
                    IPU SIMULATOR (DUT)
                              |
         +--------------------+-------------------+
         |                    |                   |
         v                    v                   v
     RX / DMA             Parser / VLAN        L2 / TX
     Descriptors          Classification       Descriptors
         |                    |                   |
         +--------------------+-------------------+
                              |
                              v
                    OBSERVED RESULTS
                              |
                              v
                  CHECKS + TEST RESULT
                              |
                              v
                  REPORT / COVERAGE / CI
```

### 4.1 Recommended logical modules

| Module | Responsibility | Must not do |
|---|---|---|
| Test registry/runner | Discover, order, run and report test cases | Implement packet-processing behavior |
| Test fixture/context | Create a known DUT and common test resources | Hide important setup from the test author |
| Configuration builder | Create explicit port, queue and pipeline configurations | Silently rely on defaults that affect outcomes |
| Packet factory | Build valid and intentionally malformed packets | Decide what the DUT should do with them |
| Traffic driver | Submit packets/events to the DUT | Decide whether the DUT passed |
| Fault controller | Arm and trigger controlled fault points | Modify normal behavior when disabled |
| Reference model | Independently calculate expected results | Call DUT internals to get expected values |
| Monitor | Observe events, descriptors, counters and packet outputs | Mutate DUT state |
| Scoreboard/checker | Compare expected and actual observations | Generate the DUT's expected output using DUT code |
| Coverage collector | Record scenarios and bins exercised | Treat execution count as functional coverage |
| Reporter | Emit concise and machine-readable results | Drop evidence needed to reproduce a failure |

### 4.2 Where these modules can live

A possible layout, aligned with the current repository, is:

```text
include/
  core/
    test.hpp
    test_result.hpp
    test_runner.hpp
  validation/
    test_context.hpp
    test_fixture.hpp
    requirement_id.hpp
    coverage.hpp
    scoreboard.hpp
    event_log.hpp
  testbench/
    packet_factory.hpp
    traffic_driver.hpp
    fault_controller.hpp
    monitors/
      rx_monitor.hpp
      tx_monitor.hpp
      descriptor_monitor.hpp
  reference_model/
    ethernet_reference.hpp
    vlan_reference.hpp
    forwarding_reference.hpp

src/
  core/
  validation/
  testbench/
  reference_model/
  tests/

tests/
  hardware/
    ... existing component tests ...

docs/
  validation/
    requirements.md
    test_plan.md
    test_cases/
    coverage.md
    known_limitations.md
```

This is a target organization, not a requirement to move all existing files immediately. Introduce directories incrementally and preserve existing working tests.

---

## 5. Requirements management and traceability

A requirement is the starting point for a validation obligation. Give each requirement a stable ID.

### 5.1 Requirement format

```text
Requirement ID: REQ-RX-001
Title: Successful RX descriptor completion
Description:
  When an enabled RX queue has an available posted descriptor and receives
  a frame that fits the associated buffer, the device shall write the frame
  into that buffer and report a completion with the correct length.
Rationale:
  The host must be able to identify and consume received frames safely.
Observable evidence:
  Completion status, completion length, packet bytes, descriptor state.
Assumptions:
  FCS handling is defined by the simulator's packet representation.
Out of scope:
  Physical-layer reception and real PCIe signaling.
```

### 5.2 Traceability matrix example

| Requirement | Test IDs | Level | Expected evidence |
|---|---|---|---|
| `REQ-RX-001` Successful RX completion | `TC-RX-001`, `TC-RX-002` | Block / integration | Completion, length, bytes and ownership |
| `REQ-RX-002` No descriptor available | `TC-RX-010` | Block | Documented drop/backpressure outcome and counters |
| `REQ-PARSE-001` Reject truncated Ethernet header | `TC-PARSE-001` | Unit / block | Parser error; no later stage execution |
| `REQ-VLAN-001` Classify configured VLAN | `TC-VLAN-001`, `TC-VLAN-002` | Block / integration | VLAN metadata and forwarding decision |
| `REQ-TX-001` TX completion lifecycle | `TC-TX-001`, `TC-TX-010` | Block | Descriptor state transitions and reclamation |

A requirement may map to many tests, and a test may provide evidence for more than one requirement. The mapping should be explicit rather than inferred from filenames.

### 5.3 Requirement quality checklist

- [ ] Uses observable language (`shall`, `must`, a defined result).
- [ ] Defines preconditions and configuration.
- [ ] Defines boundary conditions or points to them.
- [ ] Defines what happens on invalid input or resource exhaustion.
- [ ] Defines counters, status or events that expose the result.
- [ ] States assumptions and behavior that is out of scope.
- [ ] Has one or more linked test cases.

---

## 6. Test case specification

Every nontrivial test should have a specification before implementation. Use a consistent template.

### 6.1 Test case template

```text
Test ID:
Requirement ID(s):
Title:
Test level: unit | block | integration | system | performance | stress
Priority: P0 | P1 | P2 | P3
Purpose:
Preconditions:
DUT configuration:
Stimulus:
Expected result / oracle:
Observations to capture:
Pass criteria:
Failure diagnostics:
Cleanup / postconditions:
Tags:
Random seed (if applicable):
```

### 6.2 Example: truncated Ethernet header

```text
Test ID: TC-PARSE-001
Requirement ID: REQ-PARSE-001
Title: Reject a frame shorter than the Ethernet header
Test level: parser unit / block
Priority: P1
Purpose: Verify that a truncated Ethernet header is rejected safely.
Preconditions: Parser initialized; no unrelated fault is armed.
DUT configuration: Default parser configuration.
Stimulus: Supply a 13-byte frame to the Ethernet parser.
Expected result / oracle:
  The parser reports an invalid/truncated Ethernet header.
  No valid Ethernet metadata is published.
  The packet does not reach VLAN or forwarding stages.
Observations:
  Parser status, error reason, stage counters, packet state.
Pass criteria:
  Error matches the documented parser contract; no out-of-bounds access;
  no later-stage processing; resources remain valid.
Cleanup:
  Release packet buffer and restore counters/device fixture.
Tags: parser, negative, boundary
```

**Important distinction:** a 13-byte input tests a truncated Ethernet header. It is not the same as testing the Ethernet minimum frame size. A frame can have a complete 14-byte Ethernet header and still be shorter than the minimum legal on-wire frame. The simulator must document whether packet lengths include or exclude FCS and whether padding is modeled.

### 6.3 Test one behavior at a time

A test may inspect multiple related invariants, but it should have one primary behavior. Avoid one enormous test that configures VLAN, parses IPv4, performs forwarding, exhausts TX resources and resets the device. When it fails, the cause becomes ambiguous.

---

## 7. Test levels and when to use them

| Level | DUT scope | Example | Typical speed |
|---|---|---|---|
| Unit | One algorithm or class | Ethernet header parser rejects 13 bytes | Very fast |
| Component/block | One modeled hardware block plus its local state | RX ring wraps correctly at the end of the ring | Fast |
| Integration | Several connected blocks | RX descriptor → packet buffer → parser → VLAN | Medium |
| End-to-end/system | Device-level behavior | Ingress frame is forwarded to the configured egress port | Slower |
| Performance | A measured datapath and workload | Packets/s and latency for selected packet sizes | Longer |
| Stress/soak | Sustained or adversarial operation | Repeated RX/TX with queue pressure for a fixed duration or packet count | Long |

### 7.1 Test pyramid

Use many unit/component tests, a healthy number of integration tests, and fewer end-to-end/performance tests. The full-system tests are valuable but expensive to debug.

### 7.2 Example: descriptor ring wraparound

For a ring of capacity 8, test at least:

1. Empty ring.
2. One descriptor posted and consumed.
3. Exactly 8 descriptors posted.
4. Attempt to post a ninth descriptor.
5. Consume enough descriptors to move the index near the end.
6. Post and consume across index wraparound.
7. Repeat until producer and consumer have wrapped more than once.

Do not assume that a particular ring uses one empty slot to distinguish full from empty. The implementation contract must define capacity and index semantics first.

---

## 8. Packet generation and traffic stimulus

A packet factory should create explicit, reviewable packet inputs. Avoid scattering raw byte arrays and magic offsets across tests.

### 8.1 Packet factory responsibilities

- Construct Ethernet, VLAN, IPv4, IPv6, TCP and UDP headers supported by the simulator.
- Provide valid defaults and explicit overrides.
- Generate malformed/truncated packets deliberately.
- Recalculate dependent fields such as IP total length and checksum when appropriate.
- Return both packet bytes and metadata describing the construction.
- Avoid silently repairing malformed test input.

### 8.2 Example packet description

```cpp
struct PacketSpec {
    MacAddress dst;
    MacAddress src;
    std::uint16_t ether_type;
    std::optional<std::uint16_t> vlan_id;
    std::size_t payload_size;
    std::uint8_t payload_pattern;
};

// Proposed API sketch; adapt types and names to the project's actual code.
Packet make_packet(const PacketSpec& spec);
Packet make_truncated_ethernet_frame(std::size_t length);
Packet make_ipv4_packet(const Ipv4Spec& spec);
```

A useful payload pattern is deterministic and easy to recognize, for example `payload[i] = (i * 17 + 3) & 0xff`. Record the pattern or seed in test output. This is more diagnostic than a buffer filled entirely with zeroes.

### 8.3 Packet-size conventions

Document whether `Packet::size()` means:

- bytes from destination MAC through payload, excluding FCS;
- bytes including FCS;
- bytes including preamble and inter-frame gap; or
- application payload bytes only.

These are different measurements. For parser tests, packet bytes commonly exclude preamble and FCS. For line-rate calculations, wire overhead must be included. Never compare results until the convention is explicit.

### 8.4 Boundary packet matrix

For each supported parser or packet format, consider:

- Minimum complete header.
- One byte below minimum header.
- Exact minimum supported packet length.
- Maximum supported packet length.
- One byte above the configured maximum.
- Optional fields absent and present.
- Invalid length fields.
- Unsupported or unknown EtherType/protocol.
- Valid and invalid checksum cases where checksum validation is part of the specification.

Do not generate malformed fields accidentally. Each negative case should identify which field is intentionally invalid.

---

## 9. Reference models, scoreboards and oracles

A reference model is a small, independent implementation of the expected behavior. It is especially useful for packet classification, VLAN handling and forwarding decisions.

### 9.1 Reference-model rules

1. Derive behavior from the requirement/specification, not the DUT source code.
2. Keep the model simpler than the DUT where possible.
3. Avoid shared algorithms that could reproduce the same defect.
4. Use known test vectors for protocol parsing and checksum calculations.
5. Record why the reference model made a decision.
6. Review reference-model changes with the same care as DUT changes.

### 9.2 Example: simple L2 forwarding oracle

Assume the test configures a static forwarding table and the project defines known behavior for unknown unicast. The testbench can calculate the expected egress independently:

```cpp
struct ExpectedForwarding {
    bool should_forward;
    std::optional<PortId> egress;
    DropReason drop_reason;
};

ExpectedForwarding reference_l2_lookup(
    const ForwardingConfig& config,
    const EthernetHeader& header,
    PortId ingress);
```

The actual DUT output is then compared with this expected structure. This example deliberately leaves unknown-unicast, broadcast, multicast and same-port filtering behavior to the project specification; do not assume a switch policy without documenting it.

### 9.3 Scoreboard model

For a simple deterministic test, the scoreboard can hold expected packet outputs and match them to actual outputs using packet IDs or test-generated sequence numbers.

```cpp
struct ExpectedPacket {
    PacketId id;
    PortId expected_egress;
    std::vector<std::uint8_t> expected_bytes;
};

class Scoreboard {
public:
    void expect(ExpectedPacket packet);
    void observe(const ActualPacket& packet);
    CheckSummary finish() const;
};
```

For packets that can be reordered, match using stable IDs or a defined flow/sequence key rather than assuming arrival order. Define duplicate, missing and unexpected-packet checks explicitly.

### 9.4 Alternative oracles

Not every behavior needs a full reference model. Useful oracles include:

- Fixed byte-for-byte test vectors.
- Protocol specification rules.
- Mathematical invariants.
- State-machine transition tables.
- Metamorphic properties.

Example metamorphic property: changing only the source MAC of a frame should not change its parsed destination MAC or EtherType. This checks relationships between runs without needing a complete golden output for every packet.

---

## 10. Monitors, observability and evidence

A validation framework needs visibility into the modeled device. A PASS/FAIL alone is not enough.

### 10.1 Recommended observations

| Area | Useful observations |
|---|---|
| Packet | Packet ID, length, ingress/egress, bytes or hash, metadata |
| Parser | Parsed fields, status, error reason |
| RX/TX descriptors | Index, state, length, status, ownership transition |
| Buffers | Buffer ID, capacity, valid length, lease/reference count, owner |
| Pipeline | Entry/exit events, stage counters, drop stage and reason |
| Queues | Head/tail, occupancy, high-water mark, enqueue/dequeue totals |
| Fault injection | Fault ID, target, trigger count, affected packet/descriptor |
| Device | Port state, link state if modeled, reset/configuration events |

### 10.2 Event log format

Prefer structured events over unstructured print statements. A minimal event could contain:

```cpp
struct ValidationEvent {
    std::uint64_t sequence;
    std::uint64_t simulated_time;
    TestId test_id;
    PacketId packet_id;
    ComponentId component;
    EventType type;
    std::string detail;
};
```

For a simulator without a clock model, use a monotonically increasing event sequence and label it as sequence order—not physical time.

### 10.3 Counters

Define counter semantics precisely:

- Is the counter incremented on entry or successful exit?
- Does a parser error count as a packet received by the parser?
- Are retries counted separately?
- Can counters wrap, and what is their width?
- Are counters reset by device reset, software clear or neither?

Example pipeline counters:

```text
rx_packets_seen
rx_packets_completed
parser_packets_entered
parser_packets_accepted
parser_packets_dropped
vlan_packets_accepted
forwarding_packets_selected
forwarding_packets_dropped
tx_packets_submitted
tx_packets_completed
```

A counter test should verify both the expected increment and the absence of increments in stages the packet should never reach.

---

## 11. Descriptor, queue and buffer lifecycle validation

Resource lifecycle testing is central to NIC/IPU validation. A packet can be forwarded correctly while the implementation leaks or corrupts descriptors and buffers.

### 11.1 RX descriptor lifecycle

Use the lifecycle defined by the simulator. A common conceptual sequence is:

```text
HOST PREPARES BUFFER / DESCRIPTOR
              |
              v
       POSTED TO RX QUEUE
              |
              v
       DEVICE MAY CONSUME
              |
              v
       RX COMPLETION WRITTEN
              |
              v
       HOST RECLAIMS BUFFER
              |
              v
       BUFFER REPOSTED OR FREED
```

The exact ownership names and transitions must match the model. Validate legal transitions and reject or report illegal ones.

### 11.2 TX descriptor lifecycle

A common conceptual sequence is:

```text
FREE -> SUBMITTED -> DEVICE_OWNED -> COMPLETED -> RECLAIMED -> FREE
```

Tests should cover normal completion, ring-full behavior, invalid submissions, completion ordering (if specified), wraparound, reset while work is pending and repeated reuse.

### 11.3 Invariants

Examples of useful invariants:

```text
0 <= ring_occupancy <= ring_capacity
free_buffers + leased_buffers + device_owned_buffers = total_buffers
one descriptor has at most one owner
one buffer is not simultaneously returned to the pool and referenced by a packet
completed_descriptor_count <= submitted_descriptor_count
```

These equations are conceptual. Adapt them to the actual pool and ring states; some designs reserve descriptors or maintain separate free lists.

### 11.4 Buffer pool tests

At minimum, exercise:

- Allocate one buffer and release it.
- Allocate the full pool.
- Request one more than capacity.
- Release the same lease twice, if the API is supposed to detect this.
- Retain a buffer while another reference exists.
- Recycle after the last reference is released.
- Reset or destroy a pool with outstanding leases according to the documented contract.
- Repeated allocate/release loops to detect leaks and stale references.

For lease-based APIs, test lifetime and ownership semantics directly—not just whether the returned pointer is non-null.

---

## 12. Fault injection framework

Fault injection deliberately creates a controlled abnormal condition at a defined boundary. It should be separate from normal datapath policy and disabled by default.

### 12.1 Fault model

Represent a fault with explicit fields:

```cpp
enum class FaultPoint {
    RxDescriptorFetch,
    RxDmaTransfer,
    BufferAllocation,
    EthernetParser,
    VlanClassifier,
    ForwardingLookup,
    TxDescriptorSubmit,
    TxCompletion
};

struct FaultRule {
    FaultId id;
    FaultPoint point;
    std::uint64_t trigger_on_nth_hit;
    std::uint64_t remaining_triggers;
    FaultAction action;
    bool enabled;
};
```

This is a proposed schema. Keep the action specific (for example, fail allocation, reject transfer, delay completion) rather than adding an unrestricted callback that can mutate arbitrary DUT state.

### 12.2 Fault-injection principles

- Off by default; normal tests should not be affected by stale fault rules.
- Explicit target and trigger semantics.
- Deterministic trigger count and recorded seed for randomized faults.
- One-shot faults supported for focused tests.
- Fault activation and firing are both observable.
- Cleanup always disarms faults.
- Fault behavior is defined at a component boundary.
- Do not bypass safety invariants merely to force a failure.
- Verify recovery or the documented terminal state after the failure.

### 12.3 Fault scenario matrix

| Fault ID | Injection point | Example stimulus | Expected checks |
|---|---|---|---|
| `FI-RX-001` | RX descriptor fetch | No posted descriptor | Documented drop/backpressure; no buffer overwrite |
| `FI-DMA-001` | RX DMA | Fail selected transfer | Error is observable; descriptor/buffer ownership remains valid |
| `FI-BUF-001` | Buffer allocation | Pool exhausted | Allocation failure reported; no invalid pointer used |
| `FI-PARSE-001` | Ethernet parser | Truncated header | Parser rejects; downstream stages not entered |
| `FI-VLAN-001` | VLAN classifier | Force classifier error | Defined drop/error path and counter update |
| `FI-TX-001` | TX submit | TX ring full | Submit fails or backpressures as specified; no overwrite |
| `FI-TX-002` | TX completion | Suppress/delay completion | Timeout or pending state is reported; recovery behavior is checked |

Do not assign an expected result until the requirement or modeled hardware contract defines it. For example, no RX descriptor may cause drop, pause or another behavior depending on the modeled architecture.

### 12.4 First fault-injection test: truncated Ethernet header

This case can be tested without changing the production datapath if the packet factory can submit a short frame and the parser already exposes a documented error result.

**Setup:** create a fresh parser/DUT fixture; ensure no fault rules are armed.  
**Stimulus:** supply 13 bytes to an Ethernet parser that requires a 14-byte header.  
**Expected:** parser rejects the frame, does not publish a valid Ethernet header, and the integration path does not proceed to VLAN or forwarding.  
**Evidence:** parser result/error, stage-entry counters, packet state and resource cleanup.

This is input-negative testing. It is not necessarily a fault-controller-triggered fault. Keep those concepts separate: malformed input tests the DUT's input validation; fault injection forces a component failure such as a DMA error or allocation failure.

### 12.5 Test cleanup

Use scoped setup/cleanup so a failed assertion cannot leave fault rules armed. A fixture destructor or RAII guard is appropriate, provided cleanup does not hide the original failure.

---

## 13. Negative testing, boundary testing and robustness

A validation plan should include more than valid traffic.

### 13.1 Negative input categories

- Truncated Ethernet header.
- VLAN tag truncated after EtherType.
- IPv4 header shorter than its minimum.
- IPv4 IHL smaller than the minimum or larger than available bytes.
- IPv4 total length inconsistent with header length or available bytes.
- Invalid checksum, if checksum validation is required by the DUT contract.
- IPv6 payload length inconsistent with available bytes.
- Unsupported EtherType or protocol.
- Packet larger than configured maximum.
- Invalid descriptor fields.
- Invalid queue/port state.

For every parser case, define whether the test checks rejection, pass-through, metadata marking or another documented behavior. Avoid assuming policy based on protocol knowledge alone.

### 13.2 Boundary-value strategy

For a supported range `[min, max]`, test:

- `min - 1`, `min`, `min + 1`;
- representative interior values;
- `max - 1`, `max`, `max + 1`.

Also test zero, empty, full and wraparound states where meaningful. Guard against integer overflow when deriving lengths and offsets.

### 13.3 Recovery tests

After each injected or malformed condition, send a known-good packet and prove that the device continues to behave correctly when the specification says it should. This catches stale state, leaked resources and stuck queues.

---

## 14. Concurrency and ordering validation

When the simulator introduces threads or asynchronous completion, concurrency becomes part of the behavior being validated.

### 14.1 Concurrency scenarios

- One producer and one consumer.
- Multiple producers and one consumer, if supported.
- One producer and multiple consumers, if supported.
- Simultaneous RX and TX activity.
- Multiple active queues.
- Queue stop/start while work is pending.
- Reset while operations are in flight.
- Delayed and reordered completions, where the model allows them.

### 14.2 Assertions for concurrent structures

- No duplicate packet delivery unless explicitly expected.
- No missing packet in a lossless test with sufficient resources.
- No duplicate descriptor completion.
- No buffer use-after-free or double release.
- Queue occupancy and ownership invariants always hold.
- Completion ordering follows the documented contract.

### 14.3 Concurrency tooling

Use sanitizers in dedicated builds where supported:

- AddressSanitizer (ASan) for out-of-bounds and use-after-free defects.
- UndefinedBehaviorSanitizer (UBSan) for undefined operations.
- ThreadSanitizer (TSan) for data races in threaded tests.

Sanitizer builds are complementary to functional tests. A clean sanitizer run does not prove functional correctness, and sanitizer overhead makes those builds unsuitable for performance numbers.

For rare concurrency failures, record a deterministic operation schedule where possible. A seed alone may not reproduce a race if thread scheduling differs.

---

## 15. Performance validation and benchmarking

Performance testing should follow functional correctness. A fast incorrect system is not a successful result.

### 15.1 Metrics

| Metric | Definition / reporting guidance |
|---|---|
| Throughput | Report useful data rate and clarify whether Ethernet overhead is included. |
| Packets per second (PPS) | Completed or successfully processed packets per second; state which counter is used. |
| Latency | Define start/end timestamps and report percentiles (p50, p90, p99, p99.9 where sample size permits). |
| Jitter | Variation in latency, with the chosen definition stated. |
| Loss/drop rate | Lost or dropped packets divided by offered packets; separate intentional drops. |
| Queue occupancy | Average, peak and time spent near capacity. |
| Resource usage | Buffer pool occupancy, descriptor occupancy, allocations and reclamations. |
| CPU use | Only meaningful for a defined host/platform and measurement method. |

### 15.2 Workload matrix

Vary one dimension at a time before running combinations:

- Packet sizes: minimum supported, 128, 512, 1500 and jumbo sizes if supported.
- Flow count: one flow and many flows.
- Queue count: one queue through maximum supported queues.
- Direction: RX-only, TX-only and bidirectional.
- Load: low, moderate, near saturation and overload.
- Duration: short benchmark and sustained run.
- Resource size: default and intentionally constrained pools/rings.

Use only packet sizes supported by the simulator and record whether the size includes FCS.

### 15.3 Example line-rate calculation

For a 200 Gbit/s Ethernet link with minimum-size frames, a common wire-time accounting uses 64 bytes for the Ethernet frame including FCS, plus 8 bytes of preamble/SFD and 12 bytes of inter-frame gap. That is 84 byte-times per frame:

```text
PPS = 200,000,000,000 bits/s / (84 bytes × 8 bits/byte)
    ≈ 297.6 million packets/s
```

This is a theoretical line-rate calculation under those assumptions, not a performance expectation for this software simulator. State all framing assumptions in benchmark output.

### 15.4 Benchmark discipline

- Separate debug, sanitizer and optimized benchmark builds.
- Warm up before collecting measurements.
- Use a fixed workload and seed.
- Run multiple iterations and report distribution, not only the best run.
- Record compiler, build type, CPU, OS, configuration and commit.
- Avoid comparing measurements from different environments as if they were directly equivalent.
- Check correctness counters during performance runs so speed does not mask loss or corruption.

---

## 16. Test execution, results and diagnostics

### 16.1 Test result model

At minimum, distinguish:

- `PASS` — all pass criteria satisfied.
- `FAIL` — observed behavior contradicted an expected result.
- `ERROR` — the test could not execute correctly (setup exception, simulator crash, framework error).
- `SKIP` — intentionally not run, with a reason.
- `BLOCKED` — required capability or dependency unavailable.

Do not report a setup crash as a DUT functional failure. Preserve the distinction.

### 16.2 Recommended result fields

```text
run_id
build_id / git_commit
suite_name
test_id
requirement_ids
result
start/end or elapsed time
random_seed
DUT configuration
fault configuration
expected summary
actual summary
failure stage / reason
assertion details
relevant event log
coverage delta
```

### 16.3 Example failure report

```text
[FAIL] TC-RX-001 — REQ-RX-001 Successful RX descriptor completion
Seed: 0x0042A17C
Configuration: rx_queues=1, rx_ring_size=64, buffer_size=2048
Stimulus: packet_id=17, packet_length=100
Expected: completion length=100; payload bytes match
Actual:   completion length=96; payload differs at offset 72
Stage: RX completion / buffer observation
Descriptor: queue=0, index=12, state=COMPLETED
Faults armed: none
Next diagnostic: inspect DMA length calculation and packet-size convention
```

The example illustrates useful diagnostic detail; values are illustrative.

### 16.4 No silent success

A test must fail when its primary assertion is not reached. For example, a test that expects a completion but times out should not pass simply because no incorrect completion was observed. Use explicit completion counts and bounded waits.

---

## 17. Automation and continuous integration

Every code change should run a predictable set of validation checks.

### 17.1 Suggested CI stages

1. Configure CMake.
2. Build the simulator and test executables.
3. Run fast unit/component tests.
4. Run integration tests.
5. Run ASan/UBSan test build.
6. Run TSan tests separately where relevant and supported.
7. Run coverage collection.
8. Publish test results and logs.
9. Run longer stress/performance jobs on an appropriate schedule or controlled runner.

### 17.2 Local workflow for the current repository

The commands already used in the project are a reasonable starting point:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/ipu-simulator
```

If the project adds CTest registration, use:

```bash
ctest --test-dir build --output-on-failure
```

Do not assume CTest or GoogleTest is already configured. The README says GoogleTest is planned, while the repository shows a custom `core` test runner. Keep using the existing runner until a deliberate migration is approved.

### 17.3 Build configurations

Maintain separate build directories for distinct compiler options, for example:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
```

Review the actual compiler and CMake configuration before adopting these exact flags. Avoid reusing one build directory for incompatible configurations.

---

## 18. Coverage strategy

Coverage is not one number. Track several views.

### 18.1 Coverage dimensions

| Coverage type | Question |
|---|---|
| Requirement coverage | Does every approved requirement have test evidence? |
| Test-case coverage | Which planned tests have executed and passed? |
| Code coverage | Which statements, branches and functions ran? |
| Functional coverage | Which meaningful values, states and scenarios were exercised? |
| Transition coverage | Which legal and illegal state transitions were tested? |
| Fault coverage | Which fault points and recovery paths were exercised? |
| Performance coverage | Which workload/configuration points were measured? |

Code coverage is useful, but 100% line coverage does not prove that all protocol boundaries, state transitions or fault conditions have been tested.

### 18.2 Example functional bins

For an RX ring, useful bins might be:

```text
occupancy:       empty | one_entry | near_full | full
index_position:  beginning | middle | last_slot | wrapped
completion:      success | error | delayed
buffer_size:     undersized | exact_fit | larger_than_packet
```

A cross-coverage goal could ask whether each occupancy category has been tested at both ordinary and wraparound index positions. Avoid generating a huge Cartesian product without a reason; define bins that correspond to real risks.

### 18.3 Coverage closure

Before declaring a milestone complete:

- Every in-scope requirement maps to one or more tests.
- Each planned test has a recorded result.
- Important state transitions and boundaries have been exercised.
- Failures are fixed, accepted with rationale or tracked as known issues.
- Uncovered requirements and bins are documented.

---

## 19. Reliability, reset and long-run tests

Some defects appear only after many operations or during lifecycle transitions.

Recommended scenarios:

- Repeated device initialize/shutdown cycles.
- Reset with empty queues.
- Reset with descriptors pending.
- Reset with completed descriptors not yet reclaimed.
- Port disable/enable while queues are configured.
- Buffer pool exhaustion followed by recovery.
- TX completion delay followed by drain.
- Long-running mixed RX/TX traffic.
- Repeated ring wraparound.
- Repeated allocation/release under sanitizers.

Define which state survives reset and which state is cleared. Tests cannot be precise until reset semantics are documented.

---

## 20. Coding standards for the validation framework

### 20.1 Test naming

Use stable IDs and descriptive names, for example:

```text
TC-PARSE-001  Reject truncated Ethernet header
TC-RX-001     Complete RX descriptor for valid frame
TC-RX-010     Handle RX ring with no available descriptor
TC-TX-001     Complete and reclaim a TX descriptor
TC-BUF-004    Reject double release of buffer lease
TC-PIPE-020   Do not forward a packet rejected by parser
```

### 20.2 Assertion guidance

Prefer specific assertions:

```cpp
CHECK_EQ(actual_length, expected_length)
CHECK_EQ(actual_drop_reason, DropReason::TruncatedHeader)
CHECK_FALSE(tx_monitor.saw_packet(packet_id))
CHECK_EQ(buffer_pool.outstanding_leases(), 0U)
```

These macros are illustrative. Use the assertion interface implemented by `include/core/test.hpp`.

Avoid assertions that only say `CHECK(result)`. Include expected and actual values in the failure message.

### 20.3 Error handling

- Do not swallow exceptions or errors in test setup.
- Use bounded waits for asynchronous events.
- Distinguish timeout from negative completion.
- Clean up with RAII where possible.
- Ensure cleanup preserves the original failure and records cleanup failures separately.

### 20.4 Avoid fragile tests

A test is fragile when it depends on:

- Execution order of other tests.
- Unspecified timing or sleeps.
- An unseeded random generator.
- Private fields rather than observable behavior.
- Log wording instead of a structured status/event.
- A particular memory address or pointer value.
- Host scheduling assumptions not controlled by the test.

---

## 21. Worked examples

The following are framework-neutral examples. Adapt types and calls to the current code; do not paste them as-is until the project's concrete interfaces are confirmed.

### 21.1 Example A — Ethernet parser rejects a truncated header

**Oracle:** Ethernet header length is 14 bytes for the parser's packet representation. An input shorter than that cannot yield a complete Ethernet header.

```cpp
void test_truncated_ethernet_header(TestContext& ctx)
{
    constexpr std::size_t kEthernetHeaderBytes = 14;
    auto packet = ctx.packets.make_raw(kEthernetHeaderBytes - 1);

    const auto result = ctx.dut.ethernet_parser().parse(packet);

    CHECK_EQ(result.status, ParseStatus::Error);
    CHECK_EQ(result.error, ParseError::TruncatedHeader);
    CHECK_FALSE(result.header.has_value());
}
```

Additional integration assertions should verify that the packet does not enter VLAN classification or forwarding. If parser API errors are represented differently in the actual code, preserve the behavioral intent and use the real contract.

### 21.2 Example B — RX buffer receives exact packet bytes

**Preconditions:** one RX descriptor is posted with a buffer large enough for the test packet; no injected faults; queue enabled.

**Oracle:** the output buffer must contain exactly the bytes supplied to the simulated RX input, and the completion length must follow the project's documented packet-length convention.

```cpp
void test_rx_dma_copies_packet_to_posted_buffer(TestContext& ctx)
{
    const auto input = ctx.packets.make_known_ethernet_frame(100);
    auto buffer = ctx.buffers.allocate(2048);
    const auto descriptor = ctx.rx.post(buffer);

    ctx.traffic.inject_rx(input);

    const auto completion = ctx.rx.wait_for_completion(descriptor);
    CHECK(completion.has_value());
    CHECK_EQ(completion->length, input.size());
    CHECK_BYTES_EQ(buffer.bytes().first(input.size()), input.bytes());
    CHECK(ctx.rx.ownership_is_host_owned(descriptor));
}
```

A real test should additionally verify the correct completion status, descriptor index, buffer identity and resource cleanup. The method names above are illustrative.

### 21.3 Example C — TX ring rejects or handles a full ring correctly

Do not hard-code whether a full ring must drop, block or return an error. First specify the contract. Then:

1. Configure a ring with a small known capacity.
2. Submit descriptors until the documented full condition is reached.
3. Submit one additional descriptor.
4. Check the documented result.
5. Verify no existing descriptor or payload was overwritten.
6. Complete/reclaim entries.
7. Submit again and prove the ring recovers.

### 21.4 Example D — Fault a single DMA transfer and recover

1. Configure the RX queue and post enough buffers.
2. Arm a one-shot DMA failure for the next transfer.
3. Inject packet A.
4. Verify the documented DMA error, descriptor outcome and buffer ownership.
5. Verify the fault fired exactly once.
6. Inject known-good packet B.
7. Verify B is processed correctly.
8. Verify no stale fault remains armed and no resources leaked.

This test validates both failure handling and post-failure recovery.

### 21.5 Example E — End-to-end forwarding with a scoreboard

1. Configure ingress and egress ports and a known forwarding entry.
2. Construct a frame with a unique packet ID/payload pattern.
3. Ask the independent forwarding oracle for the expected egress and any required rewrite.
4. Add the expected packet to the scoreboard.
5. Inject the frame at ingress.
6. Capture egress output and feed it to the scoreboard.
7. Finish the scoreboard and assert no missing, duplicate or unexpected packets.
8. Check descriptor/buffer reclamation and per-stage counters.

---

## 22. Development workflow for each new feature

Use this sequence when adding a hardware block or validation feature:

1. **Read the current interfaces and implementation.** Confirm what already exists; do not create a duplicate abstraction.
2. **Write or identify the requirement.** Give it a stable ID and define observable behavior.
3. **Define state and invariants.** Document valid states, transitions, ownership and reset behavior.
4. **Write the test specification.** Preconditions, stimulus, oracle, observations and cleanup.
5. **Implement the smallest focused test.** Start with normal behavior and key boundaries.
6. **Implement or refine the DUT behavior.** Keep testbench logic outside the datapath.
7. **Add negative and resource-pressure cases.** Include failure recovery.
8. **Add observability.** Make failures explainable without debugging by guesswork.
9. **Run regression and sanitizers.** Confirm no unrelated subsystem regressed.
10. **Update traceability and coverage.** Record what the new tests establish and what remains untested.
11. **Review limitations.** State what the software model does not represent.

A feature is not complete merely because its happy-path test passes.

---

## 23. Suggested initial validation backlog for this repository

This is a proposed backlog based on the directories visible in the supplied tree. Check the actual interfaces and existing tests before creating duplicate cases.

### Phase A — Establish the framework contract

- [ ] Document how `Test`, `TestResult` and `TestRunner` are registered and executed.
- [ ] Standardize test IDs and requirement IDs.
- [ ] Define PASS/FAIL/ERROR/SKIP/BLOCKED semantics.
- [ ] Add structured diagnostic context to failures.
- [ ] Define fixture setup and cleanup policy.

### Phase B — Build independent testbench utilities

- [ ] Packet factory with valid and malformed frames.
- [ ] Deterministic payload/seed support.
- [ ] RX/TX and descriptor monitors.
- [ ] Buffer/lease observations.
- [ ] A basic event log and counters snapshot.
- [ ] Small reference functions for parser/VLAN/L2 expectations.

### Phase C — Strengthen component tests

- [ ] Ethernet parser lengths and EtherType boundaries.
- [ ] IPv4 IHL, total-length and checksum cases according to specification.
- [ ] IPv6 payload-length and extension-header behavior if supported.
- [ ] VLAN tagged/untagged and configured/unauthorized cases.
- [ ] RX/TX ring empty/full/wraparound and illegal transitions.
- [ ] Buffer pool exhaustion, reuse and lease lifecycle.

### Phase D — Integration and fault handling

- [ ] RX-to-parser integration.
- [ ] Parser-to-VLAN integration.
- [ ] VLAN-to-L2 forwarding integration.
- [ ] L2-to-TX integration.
- [ ] End-to-end packet scoreboard.
- [ ] Controlled DMA, buffer, descriptor and completion faults.
- [ ] Recovery after each supported failure.

### Phase E — Nonfunctional validation

- [ ] Concurrent queue tests.
- [ ] Long-run resource leak tests.
- [ ] ASan/UBSan and relevant TSan runs.
- [ ] Packet-size/flow-count/queue-count benchmark matrix.
- [ ] Coverage and regression reports.

---

## 24. Definition of done for a validation feature

A feature or milestone is ready for review when all applicable items are true:

- [ ] Requirement and expected behavior are documented.
- [ ] Test IDs and requirement traceability are recorded.
- [ ] Normal behavior is tested.
- [ ] Relevant negative and boundary behavior is tested.
- [ ] Resource ownership and cleanup are verified.
- [ ] Reset/recovery is tested where applicable.
- [ ] Observability is sufficient to diagnose failures.
- [ ] Tests are deterministic or document their nondeterminism.
- [ ] Tests are isolated and repeatable.
- [ ] Regression suite passes.
- [ ] Sanitizer checks pass where applicable.
- [ ] Coverage gaps and model limitations are recorded.
- [ ] Performance claims are backed by a reproducible benchmark, not a functional test.

---

## 25. Glossary

| Term | Meaning |
|---|---|
| DUT | Device under test; here, the IPU simulator or a modeled block. |
| Testbench | Stimulus, monitors, reference models and checkers surrounding the DUT. |
| Stimulus | Input, configuration or event applied to the DUT. |
| Oracle | Independent source of expected behavior. |
| Scoreboard | Component that compares expected transactions with observed transactions. |
| Monitor | Passive observer of DUT events or state exposed through supported interfaces. |
| Invariant | Property that must remain true throughout execution. |
| Fault injection | Deliberate introduction of a controlled failure condition. |
| Negative test | Test using invalid input or an unsupported condition to verify handling. |
| Functional coverage | Measure of meaningful behaviors, values and scenarios exercised. |
| Regression | Previously passing tests rerun after a change to detect unintended breakage. |
| Post-silicon validation | Testing actual fabricated silicon; distinct from software simulation. |
| Soak test | Long-duration test intended to expose leaks, state drift and intermittent failures. |

---

## 26. Recommended first implementation step

Do not begin by adding a large fault-injection framework. First inspect and document the real contracts in the existing custom test framework and datapath interfaces.

Start with these files:

```text
include/core/test.hpp
include/core/test_result.hpp
include/core/test_runner.hpp
include/hardware/datapath_pipeline/
include/hardware/packet_buffer/
include/hardware/rx_descriptor_ring/
include/hardware/tx_descriptor_ring/
```

Then implement one thin vertical slice:

1. One stable requirement: truncated Ethernet headers are rejected.
2. One packet factory helper for a 13-byte frame.
3. One parser test using the project's real API.
4. One assertion for the exact documented parser error.
5. One integration assertion that no downstream stage processed the packet, if the current pipeline exposes that evidence.
6. One result record that identifies the requirement and test ID.

Once that pattern is proven, reuse it for RX descriptors, packet buffers, VLAN, forwarding and fault injection.

**Guiding rule:** every test should explain what requirement it proves, every failure should explain what differed, and every modeled resource should have a verifiable lifecycle.
