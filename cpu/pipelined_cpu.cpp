#include "pipelined_cpu.hpp"
#include "handlers.hpp"

PipelinedCPU::PipelinedCPU(Memory& memory, DCache& dcache, ICache& icache)
    : dcache(dcache), icache(icache) {

    pc = memory.getFirstInstrAddr();

    for (int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }

    // initialize stack pointer (x2) to 0xFFFFFFF0
    registers[2] = 0xFFFFFFF0;

    last_instruction_addr = memory.getLastInstrAddr();
}

void PipelinedCPU::run(){
    // TODO: loop until done(). Each iteration is one clock cycle:
    // call writeback(), memory_stage(), execute_stage(), decode(), fetch()
    // in that order (see the ordering note in pipelined_cpu.hpp).
}

void PipelinedCPU::fetch(){
    // TODO: if pc <= last_instruction_addr, read icache.read(pc, 4), fill
    // if_id (instruction, pc, valid=true), advance pc. Otherwise if_id
    // should go invalid (nothing left to fetch) -- this is how the tail of
    // the pipeline knows to start draining.
}

void PipelinedCPU::decode(){
    // TODO: using if_id (last cycle's fetch result), extract opcode/funct3/
    // funct7/rs1/rs2/rd/imm via the isa:: helpers, read registers[rs1] and
    // registers[rs2], and fill id_ex. If if_id.valid is false, id_ex should
    // go invalid too (propagate the bubble).
}

void PipelinedCPU::execute_stage(){
    // TODO: dispatch on id_ex.opcode (mirror CPU::execute's switch) to
    // compute the ALU result / branch outcome / memory address from id_ex's
    // fields, and fill ex_mem.
}

void PipelinedCPU::memory_stage(){
    // TODO: if ex_mem.mem_read or ex_mem.mem_write, use dcache with
    // ex_mem.alu_result as the address (and ex_mem.rs2_data as store data),
    // and fill mem_wb.
}

void PipelinedCPU::writeback(){
    // TODO: if mem_wb.valid and mem_wb.reg_write, write the selected value
    // (mem_wb.mem_to_reg ? mem_wb.mem_data : mem_wb.alu_result) into
    // registers[mem_wb.rd]. Remember x0 must stay 0.
}

bool PipelinedCPU::done() const {
    // TODO: pipeline is fully drained (safe to stop run()) once pc has
    // passed last_instruction_addr AND all 4 pipeline registers are invalid.
    return false;
}
