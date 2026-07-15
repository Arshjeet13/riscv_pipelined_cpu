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

void Cache::loadDataToLine(uint32_t addr, CacheLine& line){
        uint32_t byte_idx = getByteIndex(addr);
        uint32_t block_start = addr - byte_idx;
        line.valid = true;
        line.tag = getTag(addr);
        memcpy(line.block, &memory[block_start], 64);
        line.lru_counter = ++timer;
}



uint8_t Cache::read(uint32_t addr){
    uint32_t set_num = getSet(addr);
    uint32_t tag     = getTag(addr);

    CacheSet& set = sets[set_num];

    int line_num = -1;
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
            line_num = i;
            break;
        }
    }

    if(line_num != -1){
        hit_count++;
        uint32_t byte_idx = getByteIndex(addr);
        CacheLine& line = set.lines[line_num];
        line.lru_counter = ++timer;

        return line.block[byte_idx];
    }
    else{
        miss_count++;

        if(empty_line_cnt != 0){
            CacheLine& line = set.lines[empty_line_idx];

            loadDataToLine(addr, line);

            return line.block[getByteIndex(addr)];
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

            loadDataToLine(addr, line);
            
            return line.block[getByteIndex(addr)];
        }
    }
}

void DCache::write(uint32_t addr, uint8_t data){
    uint32_t set_num = getSet(addr);
    uint32_t tag     = getTag(addr);

    CacheSet& set = sets[set_num];

    int line_num = -1;
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
            line_num = i;
            break;
        }
    }

    if(line_num != -1){
        hit_count++;

        uint32_t byte_idx = getByteIndex(addr);
        CacheLine& line = set.lines[line_num];
        line.lru_counter = ++timer;

    }
    else{
        miss_count++;

        if(empty_line_cnt != 0){
            CacheLine& line = set.lines[empty_line_idx];

            loadDataToLine(addr, line);
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

            loadDataToLine(addr, line);
        }
    }
}
