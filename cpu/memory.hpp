#pragma once
#include <cstdint>

class Memory {
public:
    // Sets up starting state: allocates the emulated 4GB memory space
    Memory();

    // Reads a program (one hex-encoded instruction per line) from the
    // given file and writes it into memory starting at the address
    // pc was reset to.
    void loadProgram(const char* path);

    uint8_t* data();
    uint32_t getFirstInstrAddr();
    uint32_t getLastInstrAddr();

private:
    uint8_t* memory {nullptr};
    uint32_t first_instruction_addr {0x1000};
    uint32_t last_instruction_addr  {0};
};
