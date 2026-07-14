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
    