#include "cpu/cpu.hpp"

int main() {
    CPU cpu;
    cpu.reset();
    cpu.loadProgram("tests/test1_arithmetic.hex");
    cpu.run();
    return 0;
}
