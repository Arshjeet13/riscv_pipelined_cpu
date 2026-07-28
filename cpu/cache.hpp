#pragma once
#include <cstdint>
#include <cstring>
#include <climits>
#include <cstdio>
#include <iostream>
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
    uint64_t lru_counter {0};
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
    uint32_t read (uint32_t addr, uint32_t data_len);

protected:
    uint32_t extract_bits  (int l, int r, uint32_t data);
    uint32_t getSet        (uint32_t addr);
    uint32_t getTag        (uint32_t addr);
    uint32_t getByteIndex  (uint32_t addr);

    // addr is the address of some byte beloning to the block which is going
    // to be loaded into the line. It need not be the first address of the block
    void     loadDataToLine(uint32_t addr, CacheLine& line);
    void     evictDataFromLine(uint32_t set_num, CacheLine& line);
    CacheLine&   findOrAllocateLine(uint32_t addr);
    uint8_t  readByte(uint32_t addr);
    uint16_t readHalf(uint32_t addr);
    uint32_t readWord(uint32_t addr);

    uint8_t* memory      {nullptr};
    uint64_t timer       {};
    uint64_t hit_count   {0};
    uint64_t miss_count  {0};
    uint64_t dirty_miss_count {0};
    CacheSet sets[64]    {};
};

class DCache : public Cache{

public:
    using Cache::Cache;
    void write(uint32_t addr, uint32_t data, uint32_t data_len);
    uint64_t getDirtyMissCount();
    void empty_cache();

private:
    void writeByte(uint32_t addr, uint8_t  data);
    void writeHalf(uint32_t addr, uint16_t data);
    void writeWord(uint32_t addr, uint32_t data);
    void writeDataToLine(uint8_t data, uint32_t addr, CacheLine& line);
};

class ICache : public Cache{
public:
    using Cache::Cache;
};