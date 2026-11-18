#include "compare_tool.hpp"
#include <cstring>

bool compare(CPU& cpu, Memory& memory, PipelinedCPU& p_cpu, Memory& p_memory){
    const uint32_t* cpu_regs   {cpu.get_registers()};
    const uint32_t* p_cpu_regs {p_cpu.get_registers()};

    for(int i = 0; i < REG_COUNT; ++i){
        if(cpu_regs[i] != p_cpu_regs[i]) return false;
    }

    uint8_t* mem_start = memory.data();
    uint8_t* p_mem_start = p_memory.data();

    return memcmp(mem_start, p_mem_start, MEM_SIZE) == 0;
}