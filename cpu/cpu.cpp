#include "cpu.hpp"
#include <iostream>
#include <sys/mman.h>
#include <cstdio>

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
    // read the hex file at path

    // write each decoded instruction word into memory, starting at
    // the address pc was reset to
}

void CPU::run(){
    // fetch the instruction at pc

    // decode it

    // execute it

    // update pc (sequential, branch, jump)

    // repeat until halt condition
}