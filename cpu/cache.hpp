#pragma once
#include <cstdint>
#include "memory.hpp"

class Cache {
public:
    Cache(Memory& memory);

private:
    Memory& memory;
};
