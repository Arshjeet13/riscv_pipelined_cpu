#include "cpu.hpp"
#include "handlers.hpp"
#include <iostream>
#include <sys/mman.h>
#include <cstdio>
#include <cstring>

void CPU::reset(){
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

    // zero all registers
    for (int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }

    // initialize stack pointer (x2) to 0xFFFFFFF0
    registers[2] = 0xFFFFFFF0;

    // initialize pc to the code's load address (0x1000)
    pc = 0x00001000;
}

void CPU::loadProgram(const char* path){
    freopen(path, "r", stdin);

    uint32_t instruction_addr = pc;

    std::string instruction_text;

    while(getline(std::cin, instruction_text)){
        uint32_t instruction = stoi(instruction_text);
        memcpy(&memory[instruction_addr], &instruction, 4);
        instruction_addr += 4;
    }
}

void CPU::run(){
    // fetch the instruction at pc

    // decode it

    // execute it

    // update pc (sequential, branch, jump)

    // repeat until halt condition
}

void fetch(){

}

void decode(){

}

void execute(){

}

void update_pc(){

}


