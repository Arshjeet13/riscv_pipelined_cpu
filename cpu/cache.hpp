#pragma once
#include <cstdint>
#include <cstring>
#include <climits>
#include "memory.hpp"

// Cache specs : 
// Size : 32KB
// Sets : 64
// Associatvity : 8
// Block size : 64Bytes

struct CacheLine
{
    bool     valid       {false};
    bool     dirty       {false};
    uint32_t tag         {0};
    uint8_t  block[64]   {};
    uint32_t lru_counter {0};
};

struct CacheSet
{
    CacheLine lines[8] {};
};

class Cache{
public:
    Cache(Memory& memory);
    uint64_t getHitCount();
    uint64_t getMissCount();
    uint8_t  read (uint32_t addr);

protected:
    uint32_t extract_bits  (int l, int r, uint32_t data);
    uint32_t getSet        (uint32_t addr);
    uint32_t getTag        (uint32_t addr);
    uint32_t getByteIndex  (uint32_t addr);

    // addr is the address of some byte beloning to the block which is going
    // to be loaded into the line. It need not be the first address of the block
    void     loadDataToLine(uint32_t addr, CacheLine& line);

    uint8_t* memory      {nullptr};
    uint64_t timer       {};
    uint64_t hit_count   {0};
    uint64_t miss_count  {0};
    CacheSet sets[64]    {};
};

class DCache : public Cache{

public:
    using Cache::Cache;
    void write(uint32_t addr, uint8_t data);
};

class ICache : public Cache{
public:
    using Cache::Cache;
};