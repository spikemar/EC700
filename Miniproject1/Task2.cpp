#include "pin.H"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <set>
#include <list>
#include <iomanip>


/* ===================================================================== */
/* Names of malloc and free */
/* ===================================================================== */
#if defined(TARGET_MAC)
#define CALLOC "_calloc"
#define MALLOC "_malloc"
#define FREE "_free"
#define REALLOC "_realloc"
#else
#define CALLOC "calloc"
#define MALLOC "malloc"
#define FREE "free"
#define REALLOC "realloc"
#endif

using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

class Args;

ofstream TraceFile;

Args* args = NULL;





map<ADDRINT,map<ADDRINT, int> > MEMHEAP_Malloc;
map <ADDRINT,map<ADDRINT, int> > MEMHEAP_Calloc;
map <ADDRINT,map<ADDRINT, int> > MEMHEAP_Realloc;

class Args
{
public:
    Args();
    ~Args();
    ADDRINT addr;
    ADDRINT num;
    ADDRINT size;
};

Args::Args()
{

}

Args::~Args()
{

}

/* ===================================================================== */
/* Analysis routines                                                     */
/* ===================================================================== */

VOID BeforeMalloc(ADDRINT size)
{	
    args->size = size;
}

VOID AfterMalloc(ADDRINT ret)	{
    if ( MEMHEAP_Malloc.find(ret) == MEMHEAP_Malloc.end() ) {
	MEMHEAP_Malloc[ret][args->size]=0;}
    //TraceFile << "malloc(" << args->size << ") = " << ret<< endl;
}

VOID Free(ADDRINT addr)
{
    string formatted_addr = "";
    if(addr == 0){
        formatted_addr = "0";
    } else {
        formatted_addr = addr;
    }
  //  TraceFile << "free(" + formatted_addr +") = <void>" << endl;
}

VOID BeforeCalloc(ADDRINT num, ADDRINT size)
{
    args->num = num;
    args->size = size;
}

VOID AfterCalloc(ADDRINT ret)
{
if ( MEMHEAP_Calloc.find(ret) == MEMHEAP_Calloc.end() ) {
	MEMHEAP_Calloc[ret][args->size]=0;  } 
 
//   TraceFile << "calloc(" << args->num << "," << args->size +") = " + ret << endl;
}

VOID BeforeRealloc(ADDRINT addr, ADDRINT size)
{
    args->addr = addr;
    args->size = size;
}

VOID AfterRealloc(ADDRINT ret)
{
	if ( MEMHEAP_Realloc.find(ret) == MEMHEAP_Realloc.end() ) {
		MEMHEAP_Realloc[ret][args->size]=0;}   
   // TraceFile << "realloc(" << args->addr << "," << args->size << ") = " << ret << endl;
}
static VOID RecordMem(VOID * ip, CHAR r, ADDRINT addr, ADDRINT size, BOOL isPrefetch)
{
    if (!isPrefetch)
 {if ( MEMHEAP_Malloc.find(addr) != MEMHEAP_Malloc.end() ) {
        MEMHEAP_Malloc[addr][size]++;
        }
  else if ( MEMHEAP_Calloc.find(addr) != MEMHEAP_Calloc.end() ) {
        MEMHEAP_Calloc[addr][size]++;
        }
  else if ( MEMHEAP_Realloc.find(addr) != MEMHEAP_Realloc.end() ) {
        MEMHEAP_Realloc[addr][size]++;
        }
}	
        //need something here to save address
}

static ADDRINT WriteAddr;
static ADDRINT WriteSize;

static VOID RecordWriteAddrSize(ADDRINT addr, ADDRINT size)
{
    WriteAddr = addr;
    WriteSize = size;
}


static VOID RecordMemWrite(VOID * ip)
{
    RecordMem(ip, 'W', WriteAddr, WriteSize, false);
}

VOID Instruction(INS ins, VOID *v)
{

    // instruments loads using a predicated call, i.e.
    // the call happens iff the load will be actually executed

    if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
    {
     	INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMem,
            IARG_INST_PTR,
            IARG_UINT32, 'R',
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_SIZE,
            IARG_BOOL, INS_IsPrefetch(ins),
            IARG_END);
    }

    if (INS_HasMemoryRead2(ins) && INS_IsStandardMemop(ins))
    {
     	INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMem,
            IARG_INST_PTR,
            IARG_UINT32, 'R',
            IARG_MEMORYREAD2_EA,
            IARG_MEMORYREAD_SIZE,
            IARG_BOOL, INS_IsPrefetch(ins),
            IARG_END);
    }

    // instruments stores using a predicated call, i.e.
// the call happens iff the store will be actually executed
    if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
    {
     	INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);

        if (INS_HasFallThrough(ins))
        {
            INS_InsertCall(
                ins, IPOINT_AFTER, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_END);
        }
	if (INS_IsBranchOrCall(ins))
        {
            INS_InsertCall(
                ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
         IARG_END);
        }

    }
}


/* ===================================================================== */
/* Instrumentation routines                                              */
/* ===================================================================== */
   
VOID Image(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find the malloc() function.
    RTN mallocRtn = RTN_FindByName(img, MALLOC);
    if (RTN_Valid(mallocRtn))
    {
        RTN_Open(mallocRtn);

        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)BeforeMalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)AfterMalloc,
                       IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);

        RTN_Close(mallocRtn);
    }

    // Find the free() function.
    RTN freeRtn = RTN_FindByName(img, FREE);
    if (RTN_Valid(freeRtn))
    {
        RTN_Open(freeRtn);
        // Instrument free() to print the input argument value.
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)Free,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);

        RTN_Close(freeRtn);
    }

    //Find the calloc() function
    RTN callocRtn = RTN_FindByName(img, CALLOC);
    if (RTN_Valid(callocRtn))
    {
        RTN_Open(callocRtn);

        // Instrument callocRtn to print the input argument value and the return value.
        RTN_InsertCall(callocRtn, IPOINT_BEFORE, (AFUNPTR)BeforeCalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                       IARG_END);
        RTN_InsertCall(callocRtn, IPOINT_AFTER, (AFUNPTR)AfterCalloc,
                       IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);

        RTN_Close(callocRtn);
    }
    //Find the realloc() function
    RTN reallocRtn = RTN_FindByName(img, REALLOC);
    if (RTN_Valid(reallocRtn))
    {
        RTN_Open(reallocRtn);

        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(reallocRtn, IPOINT_BEFORE, (AFUNPTR)BeforeRealloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                       IARG_END);
        RTN_InsertCall(reallocRtn, IPOINT_AFTER, (AFUNPTR)AfterRealloc,
                       IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);

        RTN_Close(reallocRtn);
    }
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
   int total=0;
   for(std::map<ADDRINT, map<ADDRINT, int> >::const_iterator it=MEMHEAP_Malloc.begin();it!=MEMHEAP_Malloc.end(); ++it) {
   TraceFile<< " Malloc Address: " << it->first << endl;
   for(std:: map<ADDRINT, int>::const_iterator it2=it->second.begin();it2!=it->second.end(); ++it2){
   TraceFile<< " size of: " << it2->first <<  " Number of: "<< it2->second<< endl;
   total+=it2->second;
	}
    TraceFile<< " Total Calls to: " << total<<  endl;
    total=0;
}

  for(std::map<ADDRINT, map<ADDRINT, int> >::const_iterator it=MEMHEAP_Calloc.begin();it!=MEMHEAP_Calloc.end(); ++it) {
   TraceFile<< " Calloc Address: " << it->first<< endl;
for(std:: map<ADDRINT, int>::const_iterator it2=it->second.begin();it2!=it->second.end(); ++it2){
   TraceFile<< " size of: " << it2->first <<  " Number of: "<< it2->second<< endl;
   total+=it2->second;
        }
    TraceFile<< " Total Calls to: " << total<<  endl;
    total=0;
}

  for(std::map<ADDRINT, map<ADDRINT, int> >::const_iterator it=MEMHEAP_Realloc.begin();it!=MEMHEAP_Realloc.end(); ++it) {
   TraceFile<< " Realloc Address: " << it->first << endl;
   for(std:: map<ADDRINT, int>::const_iterator it2=it->second.begin();it2!=it->second.end(); ++it2){
   TraceFile<< " size of: " << it2->first <<  " Number of: "<< it2->second<< endl;
   total+=it2->second;
        }
    TraceFile<< " Total Calls to: " << total<<  endl;
    total=0;
}


   TraceFile<<"END OF PINTOOL"<<endl; 
   TraceFile.close();
}

/* ===================================================================== */

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "memtrace.out", "specify trace file name");
KNOB<BOOL> KnobValues(KNOB_MODE_WRITEONCE, "pintool",
    "values", "1", "Output memory values reads and written");
/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    cerr << "This tool produces a visualisation is memory allocator activity." << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    // Initialize pin & symbol manager
    PIN_InitSymbols();
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    TraceFile.open(KnobOutputFile.Value().c_str());
    // Write to a file since TraceFile and cerr maybe closed by the application
    Args* initial = new Args();
    args = initial;
    // Register Image to be called to instrument functions.

    IMG_AddInstrumentFunction(Image, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    RecordMemWrite(0);
    RecordWriteAddrSize(0, 0);
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
