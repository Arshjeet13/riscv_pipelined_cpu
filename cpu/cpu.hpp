#pragma once
#include "cache.hpp"
#include "memory.hpp"
#include <cstdint>

class CPU {
public:
    // Sets up starting state: allocates the emulated 4GB memory space,
    // zeroes all registers, and initializes pc/sp to their reset values.
    CPU(Memory& memory, DCache& dcache, ICache& icache);

    // The fetch decode execute loop. It will runs until all instructions are processed.
    void run();

    uint32_t fetch();

    uint8_t decode(uint32_t instruction);

    void execute(uint32_t instruction, uint8_t opcode);

    void update_pc();

private:
    void getLastInstrAddr(Memory memory);

    DCache& dcache;
    ICache& icache;
    uint32_t registers[32] = {};
    uint32_t pc {0};
    uint32_t last_instruction_addr {0};
};

