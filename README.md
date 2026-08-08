# RISC-V (RV32I) Pipelined CPU Emulator

A C++ emulator for RV32I (the base 32-bit integer RISC-V ISA), built up in phases from a single-cycle model to a pipelined CPU with a cache hierarchy and a gshare branch predictor.

## Overview

The finished emulator will load and run RV32I programs in two modes — single-cycle and pipelined — and produce identical final register/memory state in both, which is the correctness proof for the pipeline. In pipelined mode it reports cycle count, IPC, data-hazard stall counts, and branch misprediction rate, and compares a static "always not-taken" branch predictor against a gshare dynamic predictor on the same branch-heavy programs to show a measured improvement in misprediction rate. A cache sits between the CPU and memory throughout, reporting hit/miss statistics.

## Status

- **Phase 0-1 (single-cycle CPU): complete.** Fetch/decode/execute loop covering R, I (imm + load), S, B, J, U instruction formats.
- **Phase 2 (cache): complete.** `Memory`/`Cache` split with separate icache/dcache, true-LRU replacement, write-back with dirty-bit tracking, byte/half/word access including cross-line boundaries, hit/miss/hit-rate reporting, flush-on-halt, and cycle-cost accounting.
- **Phase 3 (pipelining), Phase 4 (gshare branch prediction): not started.**

## Build

```
mkdir build && cd build
cmake ..
cmake --build .
```

## Design Choices

### Phase 1

- Dispatch via `switch`/`case` on opcode/funct3/funct7, not a function-pointer table — simpler given the per-case decode logic RV32I needs.
- PC updates after `execute()`, not during `fetch()` — branch/jump targets are relative to the original instruction address.

### Phase 2

- `Memory` owns the backing array; `Cache` borrows a reference, doesn't allocate.
- Cache geometry: 32KB, 64 sets, 8-way associative, 64-byte blocks.
- Replacement policy: true LRU — no die-area constraint in a software model, so no reason to approximate with pseudo-LRU.
- Separate icache/dcache — avoids a structural hazard once Phase 3 pipelines fetch and memory access into the same cycle.
