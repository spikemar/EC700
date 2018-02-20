#include <iostream>
#include <fstream>
#include "pin.H"
using namespace std;
ofstream OutFile;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;


class Stack
{
private: 
ADDRINT *p;
int top;
int length;

public:
	Stack(int=0);
	~Stack();
	void push(ADDRINT);
	ADDRINT pop();
};
Stack::Stack(int size)
{
    top=-1;
    length=size;
    if(size == 0)
        p = 0; 
    else
        p=new ADDRINT[length];
}
 
Stack::~Stack()
{
    if(p!=0)
        delete [] p;
}
 
void Stack::push(ADDRINT elem)
{
        top++;
        p[top]=elem;
}
ADDRINT Stack::pop()
{
    ADDRINT ret=p[top];
    top--;
    return ret;
}
Stack call_ret(10000);
// This function is called before every instruction is executed call
VOID docount(ADDRINT TARGET,ADDRINT RETURN, ADDRINT ip) {

icount++;
OutFile << "Address of call: " << RETURN << " count " << icount << endl;
call_ret.push(RETURN);
 }

//ret execution
VOID docount2(ADDRINT TARGET, ADDRINT id) {
	ADDRINT temp= call_ret.pop();
	if(TARGET==temp)
        OutFile << "MATCH for return of this address: " << temp  << endl;
	else
              OutFile << "NO MATCH " << temp << " TARGET is "<< TARGET << endl;
 }

VOID Instruction(INS ins, VOID *v)
{
    if(INS_IsCall(ins)){ 
   INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)docount, IARG_BRANCH_TARGET_ADDR, IARG_RETURN_IP, IARG_THREAD_ID, IARG_END);
	}
if(INS_IsRet(ins)){
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount2,IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID, IARG_END);
        }
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "inscountx.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    OutFile.setf(ios::showbase);
    OutFile << "Count " << icount << endl;
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */
int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
