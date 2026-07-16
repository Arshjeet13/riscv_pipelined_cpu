#include "cpu/cpu.hpp"
#include "cpu/memory.hpp"
#include "cpu/cache.hpp"

int main() {
    Memory memory;
    memory.loadProgram("tests/test1_arithmetic.hex");

    DCache dcache(memory);
    ICache icache(memory);

    CPU cpu(memory, dcache, icache);
    cpu.run();

    return 0;
}
