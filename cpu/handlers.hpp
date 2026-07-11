#include <cstdint>

namespace isa
{

    void writeRegister(uint32_t* registers, int rd, uint32_t result);

    namespace R
    {
        void add   (uint32_t instruction, uint32_t* registers);
        void sub   (uint32_t instruction, uint32_t* registers);
        void xor_  (uint32_t instruction, uint32_t* registers);
        void or_   (uint32_t instruction, uint32_t* registers);
        void and_  (uint32_t instruction, uint32_t* registers);
        void sll   (uint32_t instruction, uint32_t* registers);
        void srl   (uint32_t instruction, uint32_t* registers);
        void sra   (uint32_t instruction, uint32_t* registers);
        void slt   (uint32_t instruction, uint32_t* registers);
        void sltu  (uint32_t instruction, uint32_t* registers);
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