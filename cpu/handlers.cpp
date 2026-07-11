#include "handlers.hpp"

namespace isa
{

    // Extracts bits [l:r] (inclusive) from instruction as a plain integer.
    // Shift-then-mask, with width computed explicitly, so the l == 31 case
    // (funct7) never has to compute 1 << 32 -- that shift amount equals the
    // type's bit width and is undefined behavior in C++.
    int extract_bits(int l, int r, uint32_t instruction){
        int width = l - r + 1;
        uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
        return static_cast<int>((instruction >> r) & mask);
    }

    void writeRegister(uint32_t result, int rd, uint32_t* registers){
        if(rd == 0) return;
        registers[rd] = result;
    }

    namespace R
    {
        const uint16_t ADD  {0x0000};
        const uint16_t SUB  {0x0020};
        const uint16_t XOR  {0x0400};
        const uint16_t OR   {0x0600};
        const uint16_t AND  {0x0700};
        const uint16_t SLL  {0x0100};
        const uint16_t SRL  {0x0500};
        const uint16_t SRA  {0x0520};
        const uint16_t SLT  {0x0200};
        const uint16_t SLTU {0x0300};

        std::pair<int,int> get_source_registers(uint32_t instruction){
            int rs1{extract_bits(19, 15, instruction)};
            int rs2{extract_bits(24, 20, instruction)};
            return {rs1, rs2};
        }
        int get_destination_register(uint32_t instruction){
            int rd{extract_bits(11, 7, instruction)};
            return rd;
        }

        void add   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val + rs2_val, rd, registers);
        }
        void sub   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val - rs2_val, rd, registers);
        }
        void xor_  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val ^ rs2_val, rd, registers);
        }
        void or_   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val | rs2_val, rd, registers);
        }
        void and_  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister(rs1_val & rs2_val, rd, registers);
        }
        void sll   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(rs1_val << shamt, rd, registers);
        }
        void srl   (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(rs1_val >> shamt, rd, registers);
        }
        void sra   (int rs1, int rs2, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]); 
            uint32_t shamt = registers[rs2] & 0x1F;
            writeRegister(static_cast<uint32_t>(rs1_val >> shamt), rd, registers);
        }
        void slt   (int rs1, int rs2, int rd, uint32_t* registers){
            int32_t rs1_val = static_cast<int32_t>(registers[rs1]);
            int32_t rs2_val = static_cast<int32_t>(registers[rs2]);
            writeRegister((rs1_val < rs2_val) ? 1 : 0, rd, registers);
        }
        void sltu  (int rs1, int rs2, int rd, uint32_t* registers){
            uint32_t rs1_val = registers[rs1];
            uint32_t rs2_val = registers[rs2];
            writeRegister((rs1_val < rs2_val) ? 1 : 0, rd, registers);
        }

        void handle_R_type_instr(uint32_t instruction, uint32_t* registers){
            auto [rs1, rs2] = get_source_registers(instruction);
            int rd = get_destination_register(instruction);

            uint16_t funct7{static_cast<uint16_t>(extract_bits(31, 25, instruction))};
            uint16_t funct3{static_cast<uint16_t>(extract_bits(14, 12, instruction))};
            uint16_t code = (funct3 << 8) | funct7;

            switch(code)
            {
            case ADD:
                add(rs1, rs2, rd, registers);
                break;
            case SUB:
                sub(rs1, rs2, rd, registers);
                break;
            case XOR:
                xor_(rs1, rs2, rd, registers);
                break;
            case OR:
                or_(rs1, rs2, rd, registers);
                break;
            case AND:
                and_(rs1, rs2, rd, registers);
                break;
            case SLL:
                sll(rs1, rs2, rd, registers);
                break;
            case SRL:
                srl(rs1, rs2, rd, registers);
                break;
            case SRA:
                sra(rs1, rs2, rd, registers);
                break;
            case SLT:
                slt(rs1, rs2, rd, registers);
                break;
            case SLTU:
                sltu(rs1, rs2, rd, registers);
                break;
            default:
                break;
            }
        }
    }

    namespace I
    {
    
    }

    namespace S
    {
    
    }

    namespace B
    {
     
    }

    namespace J
    {
         
    }

    namespace U
    {
       
    }
}
