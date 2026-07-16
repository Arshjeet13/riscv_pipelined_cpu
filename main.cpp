#include "cpu/cpu.hpp"
#include "cpu/memory.hpp"
#include "cpu/cache.hpp"
#include <iostream>

void printCacheStats(const char* label, Cache& cache) {
    uint64_t hits = cache.getHitCount();
    uint64_t misses = cache.getMissCount();
    uint64_t total = hits + misses;
    double hit_rate = total ? static_cast<double>(hits) / total : 0.0;

    std::cout << label << ": hits=" << hits << " misses=" << misses
              << " hit_rate=" << hit_rate << "\n";
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

    return 0;
}
