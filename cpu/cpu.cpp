#include "cpu.hpp"
#include "handlers.hpp"
#include <iostream>
#include <sys/mman.h>
#include <cstdio>
#include <cstring>


CPU::CPU(Memory& memory, DCache& dcache, ICache& icache) : dcache(dcache), icache(icache) {

    // initialize pc to the code's load address. Deafault value : (0x1000)
    pc = memory.getFirstInstrAddr();

    // zero all registers
    for (int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }

    // initialize stack pointer (x2) to 0xFFFFFFF0
    registers[2] = 0xFFFFFFF0;

    getLastInstrAddr(memory);
}

void CPU::getLastInstrAddr(Memory& memory){
    last_instruction_addr = memory.getLastInstrAddr();
}

void CPU::run(){
    // instruction needs the PC to reflect it's own memory address
    // next_pc is needed since instructions may be of type JMP, i.e. adding an offset to PC
    // offset may be 0 as well
    while(pc <= last_instruction_addr){
        next_pc = pc + 4;
        uint32_t instruction = fetch();
        uint8_t opcode = decode(instruction);
        execute(instruction, opcode);
        pc = next_pc;
    }

    dcache.empty_cache();
}

uint32_t CPU::fetch(){
    uint32_t instruction;
    uint32_t instr_addr = pc;
    instruction = icache.read(instr_addr, 4);
    return instruction;
}

uint8_t CPU::decode(uint32_t instruction){
    uint8_t opcode = isa::get_opcode(instruction);
    return opcode;
}

void CPU::execute(uint32_t instruction, uint8_t opcode){
    switch (opcode)
    {
    case isa::op_R:
        isa::R::handle_instr(instruction, registers);
        break;
    case isa::op_I_IMM:
        isa::I_IMM::handle_instr(instruction, registers);
        break;
    case isa::op_I_MEM:
        isa::I_MEM::handle_instr(instruction, registers, dcache);
        break;
    case isa::op_I_JMP:
        isa::I_JMP::handle_instr(instruction, registers, pc, next_pc);
        break;
    case isa::op_S:
        isa::S::handle_instr(instruction, registers, dcache);
        break;
    case isa::op_B:
        isa::B::handle_instr(instruction, registers, pc, next_pc);
        break;
    case isa::op_J:
        isa::J::handle_instr(instruction, registers, pc, next_pc);
        break;
    case isa::op_U1:
        isa::U::handle_instr(instruction, registers, pc, opcode);
        break;
    case isa::op_U2:
        isa::U::handle_instr(instruction, registers, pc, opcode);
        break;
    default:
        break;
    }
}

const uint32_t* CPU::get_registers() const{
    return registers;    
}