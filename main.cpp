#include "cpu/cpu.hpp"
#include "cpu/memory.hpp"
#include "cpu/cache.hpp"
#include <iostream>

constexpr uint64_t HIT_COST = 1;
constexpr uint64_t MISS_COST = 100;
constexpr uint64_t WRITEBACK_COST = 99;

void printCacheStats(const char* label, Cache& cache) {
    uint64_t hits = cache.getHitCount();
    uint64_t misses = cache.getMissCount();
    uint64_t total = hits + misses;
    double hit_rate = total ? static_cast<double>(hits) / total : 0.0;

    std::cout << label << ": hits=" << hits << " misses=" << misses
              << " hit_rate=" << hit_rate << "\n";
}

void printTotalCycleCost(ICache& icache, DCache& dcache) {
    uint64_t icache_cycles = icache.getHitCount() * HIT_COST
                            + icache.getMissCount() * MISS_COST;

    uint64_t dcache_misses = dcache.getMissCount();
    uint64_t dirty_misses = dcache.getDirtyMissCount();
    uint64_t clean_misses = dcache_misses - dirty_misses;
    uint64_t flushed_lines = dcache.getFlushedLineCount();

    uint64_t dcache_cycles = dcache.getHitCount() * HIT_COST
                            + clean_misses * MISS_COST
                            + dirty_misses * (MISS_COST + WRITEBACK_COST)
                            + flushed_lines * WRITEBACK_COST;

    std::cout << "icache cycle cost: " << icache_cycles << "\n";
    std::cout << "dcache cycle cost: " << dcache_cycles
              << " (flush cost: " << flushed_lines * WRITEBACK_COST << ")\n";
    std::cout << "total cycle cost: " << (icache_cycles + dcache_cycles) << "\n";
}

int main() {
    Memory memory;
    memory.loadProgram("tests/riscv_test_program_1.hex");

    DCache dcache(memory);
    ICache icache(memory);

    CPU cpu(memory, dcache, icache);
    cpu.run();

    printCacheStats("icache", icache);
    printCacheStats("dcache", dcache);
    std::cout << "dcache: dirty_misses=" << dcache.getDirtyMissCount() << "\n";
    printTotalCycleCost(icache, dcache);

    return 0;
}
