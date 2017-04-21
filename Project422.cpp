#include <iostream>

#include "pin.H"
#include <fstream>

//typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

#include "cache.H"

using namespace std;

std::ostream * out = &cerr;

// --------------------------------------------------------------------------------
// Declarations
// --------------------------------------------------------------------------------
static UINT32 hits = 0;         // L3 Cache hits
static UINT32 misses = 0;       // L3 Cache misses
static UINT32 acc = 0;       // L3 Cache misses

UINT64 fast_forward_count = 0;  //fast forward count
UINT64 noOfinsToexcute = 1000000000;
UINT64 icount = 0;                    //number of dynamically executed instructions
// UINT64 replacement = 0;
// --------------------------------------------------------------------------------
// Output Options
//LOCALFUN VOID Fini(int code, VOID * v)
//{
    //std::cerr << itlb;
    //std::cerr << dtlb;
    //std::cerr << il1;
    //std::cerr << dl1;
    //std::cerr << ul2;
    //std::cerr << ul3;
//}
// --------------------------------------------------------------------------------

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool",
    "o", "", "specify file name for MyPinTool output");

KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0", "number of instructions to fast forward in billions");
// KNOB<UINT64> KnobReplacement(KNOB_MODE_WRITEONCE, "pintool", "s", "0", "replacement policy to use for LLC");

INT32 Usage()
{
    cerr << "This tool prints out the number of dynamically executed " << endl <<
            "instructions, basic blocks and threads in the application." << endl << endl;

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}

VOID output()
{
    *out <<  "=========== CS422: Project ====================" << endl;
    *out <<  "===============================================" << endl;
    *out <<  "                  CACHE Stats" << endl;
    *out <<  "===============================================" << endl;
    *out <<  "Instructions executed: " << icount << endl;
    *out <<  "FastForward: " << fast_forward_count << endl;
    *out <<  "No of ins to Excute: " << noOfinsToexcute << endl;
    // if (replacement == 0){
    // *out <<  "================= Round Robin =================" << endl;
    // }
    // else if (replacement == 1){
    // *out <<  "==================== LRU ======================" << endl;
    // }
    // else if (replacement == 2){
    // *out <<  "=================== SRRiP =====================" << endl;
    // }
    // else if (replacement == 3){
    // *out <<  "=================== SHiP ======================" << endl;
    // }
    *out <<  "CACHE Accesses: " << acc << endl;
    *out <<  "L3 CACHE hits: " << hits << endl;
    *out <<  "L3 CACHE misses: " << misses << endl;
    // *out <<  "SRRiP CACHE Accesses: " << acc1 << endl;
    // *out <<  "L3 SRRiP CACHE hits: " << hits1 << endl;
    // *out <<  "L3 SRRiP CACHE misses: " << misses1 << endl;
}
void MyExitRoutine() {
    output();
    exit(0);
}
VOID Fini(INT32 code, VOID *v)
{    
    output();
}
ADDRINT FastForward(void) {
    //return (icount >= fast_forward_count && icount < fast_forward_count + 1000000000);
     return (icount >= fast_forward_count);
}
ADDRINT Terminate(void)
{
    // return (icount >= fast_forward_count + 1000000000);
     return (icount >= fast_forward_count + noOfinsToexcute);
}

VOID ins_count(){
    icount++;
}
// ----------------------------------------------------------------------------------------
// Cache
// A three-level cache hierarchy with L1 instruction cache being a 32 KB
// 8-way 64-byte block size, LRU. L1 data cache is a 32 KB 8-way 64-byte block
// size, LRU. L2 cache is a 256 KB 8-way 64-byte block size LRU. L3 cache is a 2
// MB 16-way 64-byte block size. This project experiments with different L3 cache
// replacement policies.
// ----------------------------------------------------------------------------------------

namespace ITLB
{
    // instruction TLB: 4 kB pages, 32 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 32 * lineSize;
    const UINT32 associativity = 32;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE;
    const UINT32 interval = 3;
    // typedef CACHE_SRRIP(max_sets, max_associativity, interval, allocation) CACHE;
}
LOCALFUN ITLB::CACHE itlb("ITLB", ITLB::cacheSize, ITLB::lineSize, ITLB::associativity);

namespace DTLB
{
    // data TLB: 4 kB pages, 32 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 32 * lineSize;
    const UINT32 associativity = 32;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE;
    const UINT32 interval = 3;
    // typedef CACHE_SRRIP(max_sets, max_associativity, interval, allocation) CACHE;
}
LOCALVAR DTLB::CACHE dtlb("DTLB", DTLB::cacheSize, DTLB::lineSize, DTLB::associativity);

namespace IL1
{
    // 1st level instruction cache: 32 kB, 64 B lines, 8-way associative
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE;
    const UINT32 interval = 3;
    // typedef CACHE_SRRIP(max_sets, max_associativity, interval, allocation) CACHE;
}
LOCALVAR IL1::CACHE il1("L1 Instruction Cache", IL1::cacheSize, IL1::lineSize, IL1::associativity);

namespace DL1
{
    // 1st level data cache: 32 kB, 64 B lines, 8-way associative
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE;
    const UINT32 interval = 3;
    // typedef CACHE_SRRIP(max_sets, max_associativity, interval, allocation) CACHE;
}
LOCALVAR DL1::CACHE dl1("L1 Data Cache", DL1::cacheSize, DL1::lineSize, DL1::associativity);

namespace UL2
{
    // 2nd level unified cache: 256 KB, 64 B lines, 8-way associative
    const UINT32 cacheSize = 256*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;
    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE;
    const UINT32 interval = 3;
    // typedef CACHE_SRRIP(max_sets, max_associativity, interval, allocation) CACHE;
}
LOCALVAR UL2::CACHE ul2("L2 Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);

namespace UL3
{
    // 3rd level unified cache: 2 MB, 64 B lines, 16-way associative
    const UINT32 cacheSize = 2*MEGA;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 16;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;
    const UINT32 interval = 15;
    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE0;
    typedef CACHE_LRU_POLICY(max_sets, max_associativity, allocation) CACHE1;
    typedef CACHE_SRRIP_HP(max_sets, max_associativity, interval, allocation) CACHE2;
    typedef CACHE_SRRIP_FP(max_sets, max_associativity, interval, allocation) CACHE3;
    typedef CACHE_ship(max_sets, max_associativity, interval, allocation) CACHE4;
}
LOCALVAR UL3::CACHE3 ul3("L3 Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);

LOCALFUN VOID Ul3Access(ADDRINT addr, UINT32 size,ADDRINT pc, CACHE_BASE::ACCESS_TYPE accessType)
{
    // thir level unified cache
    BOOL ul3Hit = FALSE;
    ul3Hit = ul3.Access(addr, size,accessType);
    // ul3Hit = ul3.Access(addr, size,pc,accessType);
    // no. of cache hits/misses
    acc++;  
    if ( ! ul3Hit)
    {
        misses++;
    }
    else
    {
        hits++;
    }
}
LOCALFUN VOID Ul2Access(ADDRINT addr, UINT32 size,ADDRINT pc, CACHE_BASE::ACCESS_TYPE accessType)
{
    // second level unified cache
    const BOOL ul2Hit = ul2.Access(addr, size, accessType);
    // third level unified cache
    if ( ! ul2Hit) {
        Ul3Access(addr, size, pc,accessType);
    }
}

LOCALFUN VOID InsRef(ADDRINT addr)
{
    const UINT32 size = 1; // assuming access does not cross cache lines
    const CACHE_BASE::ACCESS_TYPE accessType = CACHE_BASE::ACCESS_TYPE_LOAD;

    // ITLB
    itlb.AccessSingleLine(addr, accessType);

    // first level I-cache
    const BOOL il1Hit = il1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! il1Hit){
         Ul2Access(addr, size,addr, accessType);
    }
}

LOCALFUN VOID MemRefMulti(ADDRINT addr, UINT32 size,ADDRINT pc, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    dtlb.AccessSingleLine(addr, accessType);

    // first level D-cache
    const BOOL dl1Hit = dl1.Access(addr, size, accessType);

    // second level unified Cache
    if ( ! dl1Hit){
        Ul2Access(addr, size, pc,accessType);
    }
}

LOCALFUN VOID MemRefSingle(ADDRINT addr, UINT32 size, ADDRINT pc, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    dtlb.AccessSingleLine(addr, accessType);

    // first level D-cache
    const BOOL dl1Hit = dl1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! dl1Hit) { 
        Ul2Access(addr, size,pc, accessType);
    }
    
}

LOCALFUN VOID Instruction(INS ins, VOID *v)
{
    // all instruction fetches access I-cache
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR) Terminate, IARG_END);
    INS_InsertThenCall(ins, IPOINT_BEFORE,(AFUNPTR) MyExitRoutine, IARG_END);
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)ins_count, IARG_END);

    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
    INS_InsertThenCall(
        ins, IPOINT_BEFORE, (AFUNPTR)InsRef,
        IARG_INST_PTR,
        IARG_END);

    if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
    {
        const UINT32 size = INS_MemoryReadSize(ins);
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_SIZE,
            IARG_INST_PTR, 
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_LOAD,
            IARG_END);
    }

    if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
    {
        const UINT32 size = INS_MemoryWriteSize(ins);
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)FastForward, IARG_END);
        INS_InsertThenCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_INST_PTR, 
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_STORE,
            IARG_END);
    }
}

GLOBALFUN int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    fast_forward_count = KnobFastForward.Value();
    string fileName = KnobOutputFile.Value();
    if (!fileName.empty()) { 
        out = new std::ofstream(fileName.c_str());
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    // Never returns
    PIN_StartProgram();

    return 0; // make compiler happy
}
