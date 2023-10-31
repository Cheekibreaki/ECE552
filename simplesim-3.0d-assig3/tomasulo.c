
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "sim.h"
#include "decode.def"

#include "instr.h"

/* PARAMETERS OF THE TOMASULO'S ALGORITHM */

#define INSTR_QUEUE_SIZE         10

#define RESERV_INT_SIZE    4
#define RESERV_FP_SIZE     2
#define FU_INT_SIZE        2
#define FU_FP_SIZE         1

#define FU_INT_LATENCY     4
#define FU_FP_LATENCY      9

/* IDENTIFYING INSTRUCTIONS */

//unconditional branch, jump or call
#define IS_UNCOND_CTRL(op) (MD_OP_FLAGS(op) & F_CALL || \
                         MD_OP_FLAGS(op) & F_UNCOND)

//conditional branch instruction
#define IS_COND_CTRL(op) (MD_OP_FLAGS(op) & F_COND)

//floating-point computation
#define IS_FCOMP(op) (MD_OP_FLAGS(op) & F_FCOMP)

//integer computation
#define IS_ICOMP(op) (MD_OP_FLAGS(op) & F_ICOMP)

//load instruction
#define IS_LOAD(op)  (MD_OP_FLAGS(op) & F_LOAD)

//store instruction
#define IS_STORE(op) (MD_OP_FLAGS(op) & F_STORE)

//trap instruction
#define IS_TRAP(op) (MD_OP_FLAGS(op) & F_TRAP) 

#define USES_INT_FU(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_STORE(op))
#define USES_FP_FU(op) (IS_FCOMP(op))

#define WRITES_CDB(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_FCOMP(op))

/* FOR DEBUGGING */

//prints info about an instruction
#define PRINT_INST(out,instr,str,cycle)	\
  myfprintf(out, "%d: %s", cycle, str);		\
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

#define PRINT_REG(out,reg,str,instr) \
  myfprintf(out, "reg#%d %s ", reg, str);	\
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

/* VARIABLES */

//instruction queue for tomasulo
static instruction_t* instr_queue[INSTR_QUEUE_SIZE];
//number of instructions in the instruction queue
static int instr_queue_size = 0;


/*ECE552 Assignment 3 - BEGIN*/
static instruction_t* instr_queue_pop(){
  assert(instr_queue_size != 0);
  instruction_t* tmp = instr_queue[0]; // Pop from the front
  
  // Shift every element left by one position
  int i;
  for (i = 0; i < instr_queue_size - 1; i++) {
      instr_queue[i] = instr_queue[i + 1];
  }
  instr_queue[instr_queue_size - 1] = NULL; // Clear the last element after shifting
  instr_queue_size--;
  return tmp;
}

static void instr_queue_push(instruction_t* insertEntry){
  assert(instr_queue_size<INSTR_QUEUE_SIZE);
  
  instr_queue[instr_queue_size] = insertEntry; // Insert at the rear
  instr_queue_size++;
}

static bool instr_queue_full(){
  if (instr_queue_size >= INSTR_QUEUE_SIZE) { // Check if the queue is full
    return false;
  }
    return true;
}

static instruction_t* FDPipelineReg = NULL;

/*ECE552 Assignment 3 - END*/

//reservation stations (each reservation station entry contains a pointer to an instruction)
static instruction_t* reservINT[RESERV_INT_SIZE];
static instruction_t* reservFP[RESERV_FP_SIZE];

//functional units
static instruction_t* fuINT[FU_INT_SIZE];
static instruction_t* fuFP[FU_FP_SIZE];

//common data bus
static instruction_t* commonDataBus = NULL;

//The map table keeps track of which instruction produces the value for each register
static instruction_t* map_table[MD_TOTAL_REGS];

//the index of the last instruction fetched
static int fetch_index = 0;

/* FUNCTIONAL UNITS */


/* RESERVATION STATIONS */


/* 
 * Description: 
 * 	Checks if simulation is done by finishing the very last instruction
 *      Remember that simulation is done only if the entire pipeline is empty
 * Inputs:
 * 	sim_insn: the total number of instructions simulated
 * Returns:
 * 	True: if simulation is finished
 */
static bool is_simulation_done(counter_t sim_insn) {

  /* ECE552: YOUR CODE GOES HERE */

  // Check IFQ
  int i;
  for (i=0; i < INSTR_QUEUE_SIZE; i++ ){
    if(instr_queue[i]!= NULL) return false;
  }
  
  // Check RS
  for(i=0; i < RESERV_INT_SIZE; i++){
    if(reservINT[i] != NULL) return false;
    
  }
  for(i=0; i < RESERV_FP_SIZE; i++){
    if(reservFP[i] != NULL) return false;
  }

  // Check CDB
  if(commonDataBus != NULL) return false;

  return true; //ECE552: you can change this as needed; we've added this so the code provided to you compiles
}

/* 
 * Description: 
 * 	Retires the instruction from writing to the Common Data Bus
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void CDB_To_retire(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
  instruction_t* boardcast = commonDataBus;
  commonDataBus = NULL;

  // BroadCast Map Table
  int i, j;
  for(i=0; i < MD_TOTAL_REGS; i++){
    if(map_table[i]==boardcast)
      map_table[i] = NULL;
  }
  
  // BroadCast RS
  for(i=0; i < RESERV_INT_SIZE; i++){
    for(j=0; j<3; j++){
      if(reservINT[i]->Q[j]==boardcast)
        reservINT[i]->Q[j] = NULL;
    }
  }
  for(i=0; i < RESERV_FP_SIZE; i++){
    for(j=0; j<3; j++){
      if(reservFP[i]->Q[j]==boardcast)
        reservFP[i]->Q[j] = NULL;
    }
  }
}

/*
*
*/
void cleanInstr(instruction_t* instrFinished){
  bool rsInstrDeleted = false;
  bool fuInstrDeleted = false;
  int i = 0;
  for(i=0; i < RESERV_INT_SIZE; i++){
      if(reservINT[i]==instrFinished){
        reservINT[i] = NULL;
        rsInstrDeleted = true;
      }
  }
  for(i=0; i < RESERV_FP_SIZE; i++){
      if(reservFP[i]==instrFinished){
        reservFP[i] = NULL;
        rsInstrDeleted = true;
      }
  }
  
  for(i=0; i < FU_INT_SIZE; i++){
      if(fuINT[i]==instrFinished){
        fuINT[i] = NULL;
        fuInstrDeleted = true;
      }
  }
  for(i=0; i < FU_FP_SIZE; i++){
      if(fuFP[i]==instrFinished){
        fuFP[i] = NULL;
        fuInstrDeleted = true;
      }
  }
  assert(rsInstrDeleted);
  assert(fuInstrDeleted);
}

/* 
 * Description: 
 * 	Moves an instruction from the execution stage to common data bus (if possible)
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void execute_To_CDB(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
  assert(commonDataBus != NULL);
  int i;

  instruction_t* instrOldest=NULL;
  instruction_t* instrCurr=NULL;
  for(i=0; i < FU_INT_SIZE; i++){
    if((current_cycle - fuINT[i]->tom_execute_cycle) >= FU_INT_LATENCY){
      instrCurr = fuINT[i];

      // Store Case, doesnot go into CDB
      if(IS_STORE(instrCurr->op)) {
        instrCurr->tom_cdb_cycle = 0;
        cleanInstr(instrCurr);
        continue;
      }
      
      // Check CDB RaceCondi
      if(instrOldest==NULL || (instrOldest != NULL && (instrOldest->index) > (instrCurr->index))){
        instrOldest = instrCurr;
      }
    }
  }
  for(i=0; i < FU_FP_SIZE; i++){
    if((current_cycle - fuFP[i]->tom_execute_cycle) >= FU_FP_LATENCY){
      instrCurr = fuFP[i];
      
      // Check CDB RaceCondi
      if(instrOldest==NULL || (instrOldest != NULL && (instrOldest->index) > (instrCurr->index))){
        instrOldest = instrCurr;
      }
    }
  }

  // Update CDB & instr's cdb_cycle
  commonDataBus = instrOldest;
  instrOldest->tom_cdb_cycle = current_cycle;
  cleanInstr(instrOldest);
}


// able to Execute
static instruction_t* readyRSINT[RESERV_INT_SIZE];
static instruction_t* readyRSFP[RESERV_FP_SIZE];

// sort function by array[i]->index
void sortReadyRS(instruction_t* readyRS[], int size){
  int i, j;
  for (i = 0; i < size - 1; i++){
    for (j = 0; j < size - i - 1; j++){
      if(readyRS[j]->index > readyRS[j+1]->index){
        instruction_t * temp = readyRS[j];
        readyRS[j] = readyRS[j+1];
        readyRS[j+1] = temp;
      }
    }
  }
}

// clean array
void cleanReadyRS(instruction_t* readyRS[], int size){
  int i;
  for (i = 0; i < size; i++) {
      readyRS[i] = NULL;
  }
}
// push array
void pushReadyRS(instruction_t* readyRS[], instruction_t* instr, int size){
  int i;
  for (i = 0; i < size; i++) {
      if (readyRS[i] == NULL) {
          readyRS[i] = instr;
          return;
      }
  }
}

/* 
 * Description: 
 * 	Moves instruction(s) from the issue to the execute stage (if possible). We prioritize old instructions
 *      (in program order) over new ones, if they both contend for the same functional unit.
 *      All RAW dependences need to have been resolved with stalls before an instruction enters execute.
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void issue_To_execute(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */

  cleanReadyRS(readyRSFP, RESERV_FP_SIZE);
  cleanReadyRS(readyRSINT, RESERV_INT_SIZE);
  
  // check if RS Entry has Empty Q[3] && FU structural Hazard
  // deal with RS Entry RaceCondi
  int i, j;
  bool isready;
  for(i=0; i < RESERV_FP_SIZE; i++){
    isready = true;
    for(j = 0; i < 3; j++){
      if(reservFP[i]->Q[j] != NULL){
        isready = false;
        break;
      }
    }
    if(isready) pushReadyRS(readyRSFP, reservFP[i], RESERV_FP_SIZE);
  }

  for(i=0; i < RESERV_INT_SIZE; i++){
    isready = true;
    for(j = 0; i < 3; j++){
      if(reservINT[i]->Q[j] != NULL){
        isready = false;
        break;
      }
    }
    if(isready) pushReadyRS(readyRSINT, reservINT[i], RESERV_INT_SIZE);
  }

  sortReadyRS(readyRSFP, RESERV_FP_SIZE);
  sortReadyRS(readyRSINT, RESERV_INT_SIZE);
  

  // Update FU and instr execute_cycle
  i = 0;
  while(readyRSINT[i]!=NULL){
    for(j=0; j < FU_INT_SIZE; j++){
      if(fuINT[j]==NULL){
        fuINT[j] = readyRSINT[i];
        fuINT[j]->tom_execute_cycle = current_cycle;
        break;
      }
    }
    i++;
  }

  i = 0;
  while(readyRSFP[i]!=NULL){
    for(j=0; j < FU_FP_SIZE; j++){
      if(fuFP[j]==NULL){
        fuFP[j] =  readyRSFP[i];
        fuFP[j] -> tom_execute_cycle = current_cycle;
        break;
      }
    }
    i++;
  }

}

// -1 for invalid, RS Full
int returnRSfreeIndex (instruction_t * rs[], int size){
  int i;
  for (i = 0; i < size; i++){
    if (rs[i] == NULL)
      return i;
  }
  
  return -1;
}

/* 
 * Description: 
 * 	Moves instruction(s) from the dispatch stage to the issue stage
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void dispatch_To_issue(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
  
  // get from FDPipelineReg put into RS branch, INT->USES_INT_FU, FP->USES_FP_FU
  // update instr's issue_cycle
  if(USES_INT_FU(FDPipelineReg->op)){
    int ret = returnRSfreeIndex(reservINT, RESERV_INT_SIZE);
    if(ret != -1){
      reservINT[ret] = FDPipelineReg;
      reservINT[ret]->tom_issue_cycle = current_cycle;
      FDPipelineReg = NULL;
    }
  }else if(USES_FP_FU(FDPipelineReg->op)){
    int ret = returnRSfreeIndex(reservFP, RESERV_FP_SIZE);
    if(ret != -1){
      reservFP[ret] = FDPipelineReg;
      reservFP[ret]->tom_issue_cycle = current_cycle;
      FDPipelineReg = NULL;
    }
  }else{
    assert(false);
  }
}

void update_Fetch_index(instruction_trace_t* trace){
  fetch_index++;
  assert(fetch_index <= sim_num_insn);

  if (!fetch_index % INSTR_TRACE_SIZE)
    trace = trace->next;

  assert(trace != NULL);
}

/* 
 * Description: 
 * 	Grabs an instruction from the instruction trace (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	None
 */
void fetch(instruction_trace_t* trace) {

  /* ECE552: YOUR CODE GOES HERE */
    // Propose: update fetch_index and push Instr to IFQ

    // Check IFQ Full
    if(instr_queue_full()) return;

    // Check if instr Operation is Trap, get next Instr
    instruction_t* instr = &(trace->table[fetch_index % INSTR_TRACE_SIZE]);
    assert(instr == NULL);
    while(IS_TRAP(instr->op)){
      update_Fetch_index(trace);
      instr = &(trace->table[fetch_index % INSTR_TRACE_SIZE]);
      assert(instr == NULL);
    }

    // Push Instr to IFQ
    instr_queue_push(instr);
}

/* 
 * Description: 
 * 	Calls fetch and dispatches an instruction at the same cycle (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void fetch_To_dispatch(instruction_trace_t* trace, int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
  if(FDPipelineReg == NULL){ // Capture RS Full Structural Hazard
    FDPipelineReg = instr_queue_pop();
    FDPipelineReg->tom_dispatch_cycle = current_cycle;

    // instr pop is Condi/Uncondi Branch
    if(IS_COND_CTRL(FDPipelineReg->op) || IS_UNCOND_CTRL(FDPipelineReg->op)){
      FDPipelineReg = NULL;
      FDPipelineReg->tom_issue_cycle = 0;
      FDPipelineReg->tom_execute_cycle = 0;
      FDPipelineReg->tom_cdb_cycle = 0;
    }
  }

  fetch(trace);
}

/* 
 * Description: 
 * 	Performs a cycle-by-cycle simulation of the 4-stage pipeline
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	The total number of cycles it takes to execute the instructions.
 * Extra Notes:
 * 	sim_num_insn: the number of instructions in the trace
 */
counter_t runTomasulo(instruction_trace_t* trace)
{
  //initialize instruction queue
  int i;
  for (i = 0; i < INSTR_QUEUE_SIZE; i++) {
    instr_queue[i] = NULL;
  }

  //initialize reservation stations
  for (i = 0; i < RESERV_INT_SIZE; i++) {
      reservINT[i] = NULL;
  }

  for(i = 0; i < RESERV_FP_SIZE; i++) {
      reservFP[i] = NULL;
  }

  //initialize functional units
  for (i = 0; i < FU_INT_SIZE; i++) {
    fuINT[i] = NULL;
  }

  for (i = 0; i < FU_FP_SIZE; i++) {
    fuFP[i] = NULL;
  }

  //initialize map_table to no producers
  int reg;
  for (reg = 0; reg < MD_TOTAL_REGS; reg++) {
    map_table[reg] = NULL;
  }
  
  // initalize fetch_index to 1
  fetch_index = 1;

  int cycle = 1;
  while (true) {

     /* ECE552: YOUR CODE GOES HERE */

    CDB_To_retire(cycle);

    execute_To_CDB(cycle);

    issue_To_execute(cycle);

    dispatch_To_issue(cycle);

    fetch_To_dispatch(trace, cycle);

    cycle++;

    if (is_simulation_done(sim_num_insn))
      break;
  }
  
  return cycle;
}
