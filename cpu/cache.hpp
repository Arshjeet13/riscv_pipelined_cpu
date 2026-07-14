#pragma once
#include <cstdint>
#include "memory.hpp"

class Cache{
public:
    Cache(Memory& memory);
    uint64_t getHitCount();
    uint64_t getMissCount();
    
private:
    Memory& memory;
    uint64_t hit_count  {0};
    uint64_t miss_count {0};
};
