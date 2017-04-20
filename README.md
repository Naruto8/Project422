# Project422

allcache - shows all the capabilities of the cache simulator

dcache - shows all the capabilities of the cache simulator
         NB: for performance reasons we do not model the icache

dcache_xscale_config - models the cache hierarchy of and ipaq 55xx PDA
                       cache size, associativity, etc. can be changed via commanline line switches
                       NB: for performance reasons we do not model the icache

Evaluate a three-level cache hierarchy. L1 instruction cache is 32 KB
8-way 64-byte block size, LRU. L1 data cache is 32 KB 8-way 64-byte block
size, LRU. L2 cache is 256 KB 8-way 64-byte block size LRU. L3 cache is 2
MB 16-way 64-byte block size. Your project should experiment with
different L3 cache replacement policies.
