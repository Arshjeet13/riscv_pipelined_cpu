#include "memory.hpp"
#include <iostream>
#include <sys/mman.h>
#include <cstdio>
#include <cstring>
#include <cstdint>

Memory::Memory(){
    const size_t SIZE = 4ULL * 1024 * 1024 * 1024; // 4GB

    // Reserve memory
    void* region = mmap(nullptr, SIZE, PROT_NONE,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (region == MAP_FAILED) {
        perror("mmap : Error when trying to reserve memmory");
        exit(1);
    }

    // Make it readable/writable
    if (mprotect(region, SIZE, PROT_READ | PROT_WRITE) != 0) {
        perror("mprotect : Error when trying to make reserved memory readable/writable");
        exit(1);
    }

    memory = static_cast<uint8_t*>(region);
}

void Memory::loadProgram(const char* path){
    freopen(path, "r", stdin);

    uint32_t instruction_addr = first_instruction_addr;

    std::string instruction_text;

    while(getline(std::cin, instruction_text)){
        uint32_t instruction = stoul(instruction_text, nullptr, 16);
        memcpy(&memory[instruction_addr], &instruction, 4);
        instruction_addr += 4;
    }

    last_instruction_addr = instruction_addr - 4;
}

uint8_t* Memory::data(){
    return memory;
}

uint32_t Memory::getFirstInstrAddr(){
    return first_instruction_addr;
}

uint32_t Memory::getLastInstrAddr(){
    return last_instruction_addr;
}

