#pragma once
#include <cstdint>

class Memory {
public:
    void reset();
    void loadProgram(const char* path);

    uint8_t* data();
    uint32_t getLastInstructionAddr() const;

private:
    uint8_t* memory {nullptr};
    uint32_t last_instruction_addr {0};
};
