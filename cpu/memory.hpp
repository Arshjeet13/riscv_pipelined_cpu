#pragma once
#include <cstdint>

class Memory {
public:
    // Sets up starting state: allocates the emulated 4GB memory space,
    // zeroes all registers, and initializes pc/sp to their reset values.
    void reset(uint32_t* registers, uint32_t& pc);

    // Reads a program (one hex-encoded instruction per line) from the
    // given file and writes it into memory starting at the address
    // pc was reset to.
    void loadProgram(const char* path, uint32_t pc);

    uint8_t* data();
    uint32_t getLastInstructionAddr();

private:
    uint8_t* memory {nullptr};
    uint32_t last_instruction_addr {0};
};
