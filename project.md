# RISC-V CPU Emulator — Roadmap

## What this project is

A software emulator for a real CPU instruction set architecture (RV32I, the base 32-bit integer subset of RISC-V). You're not designing your own toy ISA — you're implementing a simulator for instructions that real RISC-V silicon executes, starting from the simplest possible model (one instruction fully completes before the next starts) and evolving it into a **pipelined** CPU that overlaps multiple instructions in flight simultaneously, the way every real CPU since the 1980s actually works, with a **branch predictor** guessing which way conditional branches will go before it's certain.

## What problem it solves (and why it's the right project for you)

This project doesn't solve an external problem the way the KV store does (durability, fault tolerance). Its value is different: it forces you to build, from scratch, the exact mechanism that determines how fast real code actually runs on real hardware. Two instructions that look equally simple in C++ can differ by 10-20x in execution time depending on branch predictability and cache behavior — and most software engineers never build the mental model for *why*. You'll not only build that model, you'll implement it, which is a different and much deeper kind of understanding than reading about it.

This is also specifically the gap your coursework leaves open. CS354 gave you instruction encoding, x86 assembly, calling conventions, virtual memory, and a set-associative cache simulator — real, relevant foundations — but stopped short of pipelining, hazards, and branch prediction, which is where a huge fraction of "why is this code slow" reasoning in performance-critical systems actually lives. Given that you're already building a C++ limit order book and a binary wire-protocol gateway, this project directly complements that work: understanding pipeline stalls, branch misprediction cost, and cache misses at the implementation level is exactly the intuition that lets you write hot-path C++ that doesn't fight the CPU.

## What you end up with (the concrete output)

A command-line C++ program that:
- Loads a RISC-V RV32I program (hand-assembled or cross-compiled machine code) into simulated memory
- Executes it instruction-by-instruction, either in single-cycle mode or pipelined mode, and produces **identical final register and memory state** in both — this is your correctness proof, since a pipeline that changes program outcomes is a pipeline with a bug
- In pipelined mode, reports cycle count, instructions-per-cycle (IPC), data-hazard stall counts, and branch misprediction rate
- Runs the same branch-heavy test programs (loops, conditionals) through a static "always not-taken" predictor and then through your gshare predictor, and reports a measurable, concrete drop in misprediction rate between the two — that comparison is your headline result
- (If you reach Phase 5) issues and retires two instructions per cycle instead of one, with correctly handled inter-instruction dependencies within the same cycle

Nothing here needs a UI or persistence — the deliverable is the simulator itself plus the numbers it produces on your test programs, which is what you'd walk an interviewer through.

**Total estimate: ~4-5 weeks.**

---

## Phase 0: RV32I Setup (2-3 days)

**What it is:** Reading the instruction set spec and standing up the project skeleton — no simulation logic yet.

**What you're doing concretely:**
- Read the RV32I instruction formats: R-type (register-register ops like `add`), I-type (immediates and loads), S-type (stores), B-type (branches), U-type (upper immediates), J-type (jumps) — specifically how each format packs opcode, register fields, and immediate bits differently
- Read the register file layout (`x0`–`x31`, with `x0` hardwired to zero — a RISC-V-specific quirk worth knowing early) and the calling convention naming (`ra`, `sp`, `a0`-`a7`, etc.)
- Set up the project: CMake build, and a directory layout separating the CPU core (`cpu/`), later pipeline logic (`pipeline/`), and tests (`tests/`)

**Knowledge needed:** Nothing conceptually new — you already understand "instructions are bits that get decoded" from CS354's x86 work. This phase is learning RISC-V's specific *vocabulary* (which bits mean what), not a new idea. Budget 2-3 days almost entirely for reading the spec's instruction format tables closely enough that you can decode an instruction by hand before writing decode code.

**Done when:** You can take a raw 32-bit instruction word from the spec's examples and manually identify its format, opcode, and operands on paper.

---

## Phase 1: Single-Cycle CPU (4-6 days)

**What it is:** The simplest possible working CPU — one instruction fully fetches, decodes, executes, and commits before the next one starts. No overlap, no hazards, because there's nothing to overlap yet. This becomes your **correctness oracle** for every later phase: once you build the pipelined version, you'll run the same program through both and assert the final state matches.

**What you're building:**
- A register file (32 registers, `x0` always reads as zero), a program counter, and a flat array standing in for memory
- The fetch-decode-execute loop: read the instruction at PC, decode which operation it is, execute it (ALU op, load/store, branch/jump), update registers/memory/PC, repeat
- Cover RV32I's core categories: arithmetic and immediate ops, loads/stores, branches, and jumps

**Knowledge needed:** Low novelty for you — you already understand instruction execution conceptually from CS354. This is translating that understanding into RV32I's specific instructions rather than learning the concept of "fetch-decode-execute" from scratch.

**Done when:** You hand-trace a handful of small test programs (a loop, a few branches, a couple of loads/stores) and your emulator's register/memory state matches what you compute by hand.

---

## Phase 2: Functional Memory Model + Cache (3-4 days)

**What it is:** Sitting a cache between the CPU and the flat memory array, so later phases (especially performance measurement) run against something realistic instead of instant, uniform-latency memory.

**What you're building:**
- A parameterized cache (you choose associativity, and use LRU replacement) that intercepts memory-stage accesses
- Hit/miss tracking so later phases can report cache statistics alongside pipeline statistics

**Knowledge needed:** Very low novelty — you've already built a direct-mapped and set-associative cache with hit/miss analysis in CS354's cache lab. This phase is not about re-deriving cache theory; it's implementation practice, getting a correctly-parameterized cache wired into the CPU quickly so it's just *there* for Phase 3 onward.

**Done when:** The cache reports sane hit/miss rates on a program with an obvious access pattern (e.g., a tight loop over an array should show high hit rates after warmup).

---

## Phase 3: Pipelining (2-2.5 weeks — the core of the project)

**What it is:** Upgrading from "one instruction at a time" to the classic 5-stage pipeline — Fetch, Decode, Execute, Memory, Writeback — where up to five instructions are in different stages simultaneously on every clock cycle. This is the single biggest conceptual jump in the project, and where most of your time should go.

**What you're building, and the problem each part solves:**

- **The pipeline registers themselves** — state held between stages (IF/ID, ID/EX, EX/MEM, MEM/WB) so each stage has a stable view of "what instruction am I working on this cycle." This is the new architectural idea: instructions no longer complete atomically, they *flow*.

- **Data hazards (forwarding and stalling)** — a later instruction needs a value an earlier instruction hasn't written back to the register file yet (a RAW hazard). Solve most cases with **forwarding**: routing the value directly from an earlier stage's output to a later stage's input instead of waiting for writeback. The case forwarding can't fully fix is the **load-use hazard**: a load's result isn't available until after the memory stage, so even with forwarding you need one stall cycle. This is the detail almost everyone gets wrong on the first attempt — work out the timing on paper before coding it.

- **Control hazards (branch handling, static baseline)** — a branch's direction isn't known until partway through the pipeline, but the CPU has already fetched instructions assuming a direction. Start with the simplest possible policy: always predict "not taken," and flush the pipeline (discard the wrongly-fetched instructions) when a branch turns out taken. This becomes the baseline number Phase 4 has to beat.

- **Exception/flush mechanics** — the general mechanism for cleanly discarding in-flight instructions when something invalidates them (a misprediction now; a hardware exception if you choose to extend later). Getting this right cleanly here makes Phase 4's flush-on-misprediction trivial reuse.

**Knowledge needed (this is genuinely new — budget real reading time, not just coding time):**
- The 5-stage pipeline model itself and why naive overlap breaks correctness without hazard handling
- RAW data hazards, forwarding paths, and specifically the load-use stall case
- Control hazards: why branch resolution timing forces speculation, and the flush mechanism to recover from a wrong guess
- **Recommended reading:** *Computer Organization and Design: RISC-V Edition* (Patterson & Hennessy) — it's written around your exact ISA, and its hazards/pipelining chapters are the primary source for this whole phase. Read before you implement, not alongside.

**Verification approach:** Run identical test programs through your Phase 1 single-cycle model and your new pipelined model. Final register and memory state must match exactly — any divergence means a hazard bug. This differential-testing approach is your main debugging tool for this entire phase, not just a final check.

**Done when:** Every test program (including ones deliberately loaded with back-to-back dependent instructions and tight branch loops) produces identical final state between single-cycle and pipelined execution, and you can report cycle count and IPC for each.

---

## Phase 4: Branch Prediction — Gshare (1-1.5 weeks)

**What it is:** Replacing the Phase 3 static "always not-taken" predictor with **gshare**, a real dynamic branch predictor design used in actual commercial CPUs — deliberately more ambitious than the textbook-minimum 2-bit counter, since the goal is a genuinely impressive, measurable result, not just a checkbox.

**What you're building:**
- A **global history register (GHR)**: a shift register recording the last N branch outcomes (taken/not-taken) across *all* branches, not per-branch
- A **pattern history table (PHT)** of 2-bit saturating counters, indexed by `GHR XOR PC` (the "gshare" trick — XORing spreads different branches with the same history into different table entries, and lets the *same* static branch get different predictions depending on the path taken to reach it, which is the actual insight behind why this beats a plain per-branch counter table)
- Wiring this into your Phase 3 flush mechanism: on a misprediction, flush exactly like before, but now also update the GHR/PHT with the real outcome

**Knowledge needed:**
- 2-bit saturating counters as the building block — understand these in isolation first
- Why XORing history with PC helps (the path-sensitivity argument above), not just "we index a table with something"
- Misprediction cost accounting specific to your pipeline depth — how many cycles a flush actually costs, since that's the basis for your measured improvement number
- **Recommended reading:** start from a solid lecture-notes treatment (Berkeley CS152 or CMU 15-418/447 branch prediction slides are commonly cited and clearer starting points than the original papers) before going to the original Yeh & McFarling papers if you want the primary source

**Optional stretch (only if time allows):** a tournament predictor that tracks which of gshare vs. a local per-branch predictor has been more accurate recently and switches between them — closer to what high-performance real CPUs actually do.

**Verification approach:** Run the same branch-heavy test programs (loops, nested conditionals) through the Phase 3 static predictor and the new gshare predictor, and report misprediction rate for each. The gap between them is your concrete, quotable result.

**Done when:** You have a clear before/after misprediction-rate comparison on at least a few distinct branch-pattern test programs (a tight loop, an unpredictable/random-ish branch, an alternating pattern) — gshare should clearly win on the patterns it's designed for.

---

## Phase 5 (Optional Stretch): 2-Wide Superscalar Dispatch

**Only pursue this if Phases 0-4 are solid and you have time left over.** This is graduate-level architecture territory and is explicitly a bonus.

**What it is:** Fetching and decoding two instructions per cycle instead of one, which is what starts to make this resemble a real modern core rather than a teaching pipeline.

**What you're building:**
- Dual fetch/decode per cycle
- Handling for the new hazard case this introduces: two instructions issued in the *same* cycle can depend on each other, which your Phase 3 forwarding logic wasn't designed for
- Either dual execution units or careful same-cycle scheduling to resolve that

**Knowledge needed:** Everything from Phases 1-4 solid as a prerequisite. This is new territory beyond what any single course or book chapter will hand you cleanly — treat it as open-ended exploration, not a scoped phase with a known answer.

---

## What's deliberately not in this project

- **No compiler.** Bolting one on would dilute the architecture focus and turn this into two half-finished projects instead of one deep one.
- **No exploit/security content.** That's a separate, decoupled project — real x86-64 stack exploitation against real compiled binaries, leaning on the stack/calling-convention knowledge you already have from CS354 and pushing into new territory (canaries, NX, ASLR, basic ROP) that this project doesn't touch.

---

## Rough Time Allocation

| Phase | Focus | Est. time | Novelty for you |
|---|---|---|---|
| 0 | RV32I setup | 2-3 days | Low — new ISA, familiar concept |
| 1 | Single-cycle CPU | 4-6 days | Low-medium |
| 2 | Cache (functional) | 3-4 days | Very low — you've built this |
| 3 | Pipelining | 2-2.5 weeks | **High — core learning** |
| 4 | Gshare branch prediction | 1-1.5 weeks | **High — core learning** |
| 5 (stretch) | Superscalar | open-ended | Very high, graduate-level |

**Total core scope (0-4): ~4-5 weeks.** This is the complete, resume-worthy version — a working pipelined RV32I CPU with a real dynamic branch predictor and a measured, quotable improvement over a static baseline. Phase 5 is a bonus if you want to push further.
