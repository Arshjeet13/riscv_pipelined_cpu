#pragma once
#include <cstdint>
#include <utility>

namespace isa
{

    int extract_bits(int l, int r, uint32_t instruction);
    void writeRegister(uint32_t result, int rd, uint32_t* registers);

    namespace R
    {
        std::pair<int,int> get_source_registers(uint32_t instruction);
        int get_destination_register(uint32_t instruction);

        void add   (int rs1, int rs2, int rd, uint32_t* registers);
        void sub   (int rs1, int rs2, int rd, uint32_t* registers);
        void xor_  (int rs1, int rs2, int rd, uint32_t* registers);
        void or_   (int rs1, int rs2, int rd, uint32_t* registers);
        void and_  (int rs1, int rs2, int rd, uint32_t* registers);
        void sll   (int rs1, int rs2, int rd, uint32_t* registers);
        void srl   (int rs1, int rs2, int rd, uint32_t* registers);
        void sra   (int rs1, int rs2, int rd, uint32_t* registers);
        void slt   (int rs1, int rs2, int rd, uint32_t* registers);
        void sltu  (int rs1, int rs2, int rd, uint32_t* registers);

        void handle_R_type_instr(uint32_t instruction, uint32_t* registers);
    }

    namespace I
    {
        void addi  (uint32_t instruction, uint32_t* registers);
        void xori_ (uint32_t instruction, uint32_t* registers);
        void ori_  (uint32_t instruction, uint32_t* registers);
        void andi_ (uint32_t instruction, uint32_t* registers);
        void slli  (uint32_t instruction, uint32_t* registers);
        void srli  (uint32_t instruction, uint32_t* registers);
        void srai  (uint32_t instruction, uint32_t* registers);
        void slti  (uint32_t instruction, uint32_t* registers);
        void sltiu (uint32_t instruction, uint32_t* registers);
        void lb    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void lh    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void lw    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void lbu   (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void lhu   (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void jalr  (uint32_t instruction, uint32_t* registers, uint32_t& pc);
    }

    namespace S
    {
        void sb    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void sh    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
        void sw    (uint32_t instruction, uint32_t* registers, uint8_t* memory);
    }

    namespace B
    {
        void beq   (uint32_t instruction, uint32_t* registers, uint32_t& pc);
        void bne   (uint32_t instruction, uint32_t* registers, uint32_t& pc);
        void blt   (uint32_t instruction, uint32_t* registers, uint32_t& pc);
        void bge   (uint32_t instruction, uint32_t* registers, uint32_t& pc);
        void bltu  (uint32_t instruction, uint32_t* registers, uint32_t& pc);
        void bgeu  (uint32_t instruction, uint32_t* registers, uint32_t& pc);
    }

    namespace J
    {
        void jal   (uint32_t instruction, uint32_t* registers, uint32_t& pc);
    }

    namespace U
    {
        void lui   (uint32_t instruction, uint32_t* registers);
        void auipc (uint32_t instruction, uint32_t* registers, uint32_t pc);
    }
}