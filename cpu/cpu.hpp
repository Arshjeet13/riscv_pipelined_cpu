#pragma once
#include <cstdint>

class CPU {
public:
    // Sets up starting state: allocates the emulated 4GB memory space,
    // zeroes all registers, and initializes pc/sp to their reset values.
    void reset();

    // Reads a program (one hex-encoded instruction per line) from the
    // given file and writes it into memory starting at the address
    // pc was reset to.
    void loadProgram(const char* path);

    // The fetch-decode-execute loop. Runs until a halt condition is reached.
    void run();

    uint32_t fetch();

    void decode(uint32_t instruction);

    void execute(uint32_t instruction, uint8_t opcode);

    void update_pc();

private:
    uint8_t* memory {nullptr};
    uint32_t registers[32] = {};
    uint32_t pc {0};
};

