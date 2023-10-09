#include "predictor.h"
#include <stdio.h>

void printBinary(UINT32 num) {
    for (int i = 31; i >= 0; i--) {  // Assuming a 32-bit integer
        printf("%d", (num & (1 << i)) ? 1 : 0);
        if (i % 4 == 0 && i != 0) {  // Optional: Add space every 4 bits for better readability
            printf(" ");
        }
    }
    printf("\n");
}

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
#define TAGSIZE 12
#define BPTROW 4096 // 2-bit Branch Perdictor Table Size

static int _2bitPerdT[BPTROW]; // 2-bit Branch Perdictor Table
// NN: 0 Strong Not Taken
// NT: 1 Weak Not Taken (Use as Initial Value)
// TN: 2 Weak Taken
// TT: 3 Strong Taken

void InitPredictor_2bitsat() {
  for(int i=0; i< BPTROW; i++){
    _2bitPerdT[i] = 1;
  }
}

void _12bitMask(UINT32 & PC){
  PC = PC & ((UINT32)BPTROW-1);
}

bool GetPrediction_2bitsat(UINT32 PC) {
  _12bitMask(PC);
  if(_2bitPerdT[PC] > 1)
    return TAKEN;
  else
    return NOT_TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  _12bitMask(PC);

  if(resolveDir == NOT_TAKEN && predDir == TAKEN){
    _2bitPerdT[PC]--;
  }
  if(resolveDir == TAKEN && predDir == NOT_TAKEN){
    _2bitPerdT[PC]++;
  }
  if(resolveDir == TAKEN && predDir == TAKEN && _2bitPerdT[PC] < 3){
    _2bitPerdT[PC]++;
  }
  if(resolveDir == NOT_TAKEN && predDir == NOT_TAKEN && _2bitPerdT[PC] > 0){
    _2bitPerdT[PC]--;
  }
}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////
#undef BPTROW

#define BHTTAGSIZE 9
#define BHTSIZE 512 // 2^9
#define BHRSIZE 6

#define BPTTAGSIZE 3
#define BPTSIZE 8 // 2^3
#define BPTROW 64 // 2^6

static int _6bitBHT[BHTSIZE]={0}; // use only first 6 bit, inialize with NNNNNN
static int _2bitBPT[BPTSIZE][BPTROW]; // use only first 2 bit

void _12bitTagSplit(UINT32 PC, UINT32 & _9bitTag, UINT32 & _3bitTag){
  _9bitTag = PC & ((UINT32)BHTSIZE-1);
  _3bitTag = (PC << BHTTAGSIZE) & ((UINT32)BPTSIZE-1);
}

void InitPredictor_2level() { 
  for(int i=0; i< BPTSIZE; i++){
    for(int j=0; j< BPTROW; j++){
      _2bitBPT[i][j] = 1;
    }
  }
}

bool GetPrediction_2level(UINT32 PC) {
  UINT32 _9bitTag, _3bitTag;
  _12bitTagSplit(PC, _9bitTag, _3bitTag);

  if(_2bitBPT[_3bitTag][_6bitBHT[_9bitTag]] > 1)
    return TAKEN;
  else
    return NOT_TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  UINT32 _9bitTag, _3bitTag;
  _12bitTagSplit(PC, _9bitTag, _3bitTag);

  int* _2bitPtr = &(_2bitBPT[_3bitTag][_6bitBHT[_9bitTag]]);

  if(resolveDir == NOT_TAKEN && predDir == TAKEN){
    (*_2bitPtr)--;
  }
  if(resolveDir == TAKEN && predDir == NOT_TAKEN){
    (*_2bitPtr)++;
  }
  if(resolveDir == TAKEN && predDir == TAKEN && _2bitPerdT[PC] < 3){
    (*_2bitPtr)++;
  }
  if(resolveDir == NOT_TAKEN && predDir == NOT_TAKEN && _2bitPerdT[PC] > 0){
    (*_2bitPtr)--;
  }
  

}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////

void InitPredictor_openend() {

}

bool GetPrediction_openend(UINT32 PC) {

  return TAKEN;
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {

}

