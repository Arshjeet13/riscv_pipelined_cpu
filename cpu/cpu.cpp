#include "cpu.hpp"
#include <iostream>

void CPU::reset() {
    // reserve a 4GB chunk of memory (mmap)

    // zero all registers

    // initialize stack pointer (x2) to 0xFFFFFFF0

    // initialize pc to the code's load address (0x1000)
}

void CPU::loadProgram(const char* path) {
    // read the hex file at path

    // write each decoded instruction word into memory, starting at
    // the address pc was reset to
}

void CPU::run() {
    // fetch the instruction at pc

    // decode it

    // execute it

    // update pc (sequential, branch, jump)

    // repeat until halt condition
}

