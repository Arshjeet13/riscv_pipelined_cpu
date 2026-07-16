#include "cache.hpp"

Cache::Cache(Memory& memory){
    Cache::memory = memory.data();
}

uint64_t Cache::getHitCount(){
    return hit_count;    
}

uint64_t Cache::getMissCount(){
    return miss_count;
}

uint32_t Cache::extract_bits(int l, int r, uint32_t data){
    int width = l - r + 1;
    uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
    return ((data >> r) & mask);
}

uint32_t Cache::getSet(uint32_t addr){
    return extract_bits(11, 6, addr);
}

uint32_t Cache::getTag(uint32_t addr){
    return extract_bits(31, 12, addr);
}

uint32_t Cache::getByteIndex(uint32_t addr){
    return extract_bits(5, 0, addr);
}

CacheLine& Cache::findOrAllocateLine(uint32_t addr){
    uint32_t set_num = getSet(addr);
    uint32_t tag     = getTag(addr);

    CacheSet& set = sets[set_num];

    int line_idx = -1;
    int empty_line_idx = -1;
    int empty_line_cnt = 0;

    for(int i = 0; i < 8; ++i){
        CacheLine& line = set.lines[i];
        if(!line.valid){
            empty_line_cnt++;
            empty_line_idx = i;
            continue;
        }

        if(line.tag == tag){
            line_idx = i;
            break;
        }
    }

    if(line_idx != -1){
        hit_count++;
        CacheLine& line = set.lines[line_idx];
        line.lru_counter = ++timer;
        return line;
    }

    miss_count++;

    if(empty_line_cnt != 0){
        CacheLine& line = set.lines[empty_line_idx];
        loadDataToLine(addr, line);
        return line;
    }
    else{
        uint64_t smallest = LLONG_MAX;
        uint64_t smallest_line_idx = -1;
        for(int i = 0; i < 8; ++i){
            CacheLine& line = set.lines[i];
            if(smallest > line.lru_counter){
                smallest = line.lru_counter;
                smallest_line_idx = i;
            }
        }
        
        CacheLine& line = set.lines[smallest_line_idx];
        evictDataFromLine(set_num, line);
        loadDataToLine(addr, line);
        return line;
    }    
}

void Cache::loadDataToLine(uint32_t addr, CacheLine& line){
    uint32_t byte_idx = getByteIndex(addr);
    uint32_t block_start = addr - byte_idx;
    line.valid = true;
    line.tag = getTag(addr);
    memcpy(line.block, &memory[block_start], 64);
    line.lru_counter = ++timer;
}

void Cache::evictDataFromLine(uint32_t set_num, CacheLine& line){
    if(line.dirty){
        dirty_miss_count++;
        uint32_t start_addr {};
        uint32_t tag = line.tag;
        start_addr = (tag << 12) | (set_num << 6);
        memcpy(&memory[start_addr], line.block, 64);
    }

    line.valid = false;
    line.dirty = false;
    line.tag = static_cast<uint32_t> (0);
    line.lru_counter = ++timer;
}

uint8_t Cache::readByte(uint32_t addr){
    CacheLine& line = findOrAllocateLine(addr);
    return line.block[getByteIndex(addr)];    
}

uint16_t Cache::readHalf(uint32_t addr){
    CacheLine& line1 = findOrAllocateLine(addr);
    uint32_t byte_idx1 = getByteIndex(addr);
    uint8_t byte1 = line1.block[byte_idx1];

    uint32_t byte_idx2 {};
    uint8_t byte2 {};

    if(byte_idx1 == 63){
        CacheLine& line2 = findOrAllocateLine(addr + 1);
        byte_idx2 = getByteIndex(addr + 1);
        byte2 = line2.block[byte_idx2];
    }
    else{
        byte_idx2 = byte_idx1 + 1;
        byte2 = line1.block[byte_idx2];
    }

    return static_cast<uint16_t> ((byte2 << 8) | byte1);
}

uint32_t Cache::readWord(uint32_t addr){
    CacheLine& line1 = findOrAllocateLine(addr);
    uint32_t byte_idx1 = getByteIndex(addr);

    if(byte_idx1 >= 61){
        int data_len_line1 = 64 - byte_idx1;
        int data_len_line2 = 4 - data_len_line1;

        CacheLine& line2 = findOrAllocateLine(addr + 4); // 4 is a big enough value to give us the 
                                                         // line with remaining bytes of data
        uint32_t data1 {};
        uint32_t data2 {};
        memcpy(&data1, &line1.block[byte_idx1], data_len_line1);
        memcpy(&data2, &line2.block[0],         data_len_line2);

        data2 <<= (8 * data_len_line1);
        return (data1 | data2);
    }
    else{
        uint32_t data {};
        memcpy(&data, &line1.block[byte_idx1], 4);
        return data;        
    }
}

uint32_t Cache::read(uint32_t addr, uint32_t data_len){
    switch (data_len)
    {
    case 1:
        return static_cast<uint32_t> (readByte(addr));
        break;
    
    case 2:
        return static_cast<uint32_t> (readHalf(addr));
        break;
    
    case 4:
        return static_cast<uint32_t> (readWord(addr));
        break;
    
    default:
        std::cerr << "Read request to cache was not of size 1/2/4 bytes\n";
        return -1;
        break;
    }
}

void DCache::writeDataToLine(uint8_t data, uint32_t addr, CacheLine& line){
    uint32_t byte_idx = getByteIndex(addr);
    line.dirty = true;
    line.block[byte_idx] = data;    
    line.lru_counter = ++timer;
}

void DCache::writeByte(uint32_t addr, uint8_t  data){
    CacheLine& line = findOrAllocateLine(addr);
    writeDataToLine(data, addr, line); 
}

void DCache::writeHalf(uint32_t addr, uint16_t data){
    CacheLine& line1 = findOrAllocateLine(addr);
    uint32_t byte_idx1 = getByteIndex(addr);
    uint8_t byte1 = static_cast<uint8_t> (data & (0x00FF));
    uint8_t byte2 = static_cast<uint8_t> ((data & (0xFF00)) >> 8);

    if(byte_idx1 == 63){
        CacheLine& line2 = findOrAllocateLine(addr + 1);
        writeDataToLine(byte1, addr, line1);
        writeDataToLine(byte2, addr+1, line2);        
    }
    else{
        writeDataToLine(byte1, addr, line1);
        writeDataToLine(byte2, addr + 1, line1);
    }
}

void DCache::writeWord(uint32_t addr, uint32_t data){
    CacheLine& line1 = findOrAllocateLine(addr);
    uint32_t byte_idx1 = getByteIndex(addr);
    uint8_t byte1 = static_cast<uint8_t> ( data & (0x000000FF));
    uint8_t byte2 = static_cast<uint8_t> ((data & (0x0000FF00)) >> 8);
    uint8_t byte3 = static_cast<uint8_t> ((data & (0x00FF0000)) >> 16);
    uint8_t byte4 = static_cast<uint8_t> ((data & (0xFF000000)) >> 24);

    if(byte_idx1 >= 61){
        CacheLine& line2 = findOrAllocateLine(addr + 4); // 4 is a big enough value to give us the 
                                                         // line with remaining bytes of data
        int data_len_line1 = 64 - byte_idx1;
        switch (data_len_line1)
        {
        case 1:
            writeDataToLine(byte1, addr,   line1);
            writeDataToLine(byte2, addr+1, line2);
            writeDataToLine(byte3, addr+2, line2);
            writeDataToLine(byte4, addr+3, line2);

            break;
        case 2:
            writeDataToLine(byte1, addr,   line1);
            writeDataToLine(byte2, addr+1, line1);
            writeDataToLine(byte3, addr+2, line2);
            writeDataToLine(byte4, addr+3, line2);
            break;
        
        case 3:
            writeDataToLine(byte1, addr,   line1);
            writeDataToLine(byte2, addr+1, line1);
            writeDataToLine(byte3, addr+2, line1);
            writeDataToLine(byte4, addr+3, line2);
            break;

        default:
            break;
        }              
    }
    else{
        writeDataToLine(byte1, addr,   line1);
        writeDataToLine(byte2, addr+1, line1);
        writeDataToLine(byte3, addr+2, line1);
        writeDataToLine(byte4, addr+3, line1);
    }
}

void DCache::write(uint32_t addr, uint32_t data, uint32_t data_len){

    switch (data_len)
    {
    case 1:
        writeByte(addr, data);
        break;
    case 2:
        writeHalf(addr, data);
        break;
    case 4:
        writeWord(addr, data);
        break;
    default:
        std::cerr << "Write request to Data cache was not of size 1/2/4 bytes\n";
        break;
    }   
}

uint64_t DCache::getDirtyMissCount(){
    return dirty_miss_count;
}