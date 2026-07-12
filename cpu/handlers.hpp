#pragma once
#include <cstdint>
#include <utility>

namespace isa
{

    uint32_t extract_bits(int l, int r, uint32_t data);
    void writeRegister(uint32_t result, int rd, uint32_t* registers);
    int get_source_register_1   (uint32_t instruction);
    int get_source_register_2   (uint32_t instruction);
    int get_destination_register(uint32_t instruction);
    uint32_t get_imm            (uint32_t instruction);

    namespace R
    {
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

        void handle_instr(uint32_t instruction, uint32_t* registers);
    }

    namespace I_IMM
    {
        void addi  (int rs1, int imm, int rd, uint32_t* registers);
        void xori_ (int rs1, int imm, int rd, uint32_t* registers);
        void ori_  (int rs1, int imm, int rd, uint32_t* registers);
        void andi_ (int rs1, int imm, int rd, uint32_t* registers);
        void slli  (int rs1, int imm, int rd, uint32_t* registers);
        void srli  (int rs1, int imm, int rd, uint32_t* registers);
        void srai  (int rs1, int imm, int rd, uint32_t* registers);
        void slti  (int rs1, int imm, int rd, uint32_t* registers);
        void sltiu (int rs1, int imm, int rd, uint32_t* registers);

        void handle_instr(uint32_t instruction, uint32_t* registers);
    }

    namespace I_MEM
    {
        void lb    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lh    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lw    (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lbu   (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void lhu   (int rs1, int imm, int rd, uint32_t* registers, uint8_t* memory);
        void handle_instr(uint32_t instruction, uint32_t* registers, uint8_t* memory);
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